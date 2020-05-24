#include "allocate.h"

#include <stdlib.h>
#include <string.h>
#include <generated/void_pointer_list.h>
#include <collections/pointer_set.h>

#include "errors.h"

static struct VoidPointerList *allocated_pointers = 0;
static struct PointerSet *marked_pointers = 0;

static bool garbage_collection_paused = false;

void ParserGC_init(void) {
    if (allocated_pointers) {
        fail_with_message("Attempting to re-initialize ParserGC");
    }
    allocated_pointers = VoidPointerList_create();
    marked_pointers = PointerSet_create();
    garbage_collection_paused = false;
}

void ParserGC_pause() {
    garbage_collection_paused = true;
}

void ParserGC_resume() {
    garbage_collection_paused = false;
}

void ParserGC_mark(void *pointer) {
    if (!allocated_pointers) {
        fail_with_message("ParserGC not initialized");
    }
    PointerSet_add(marked_pointers, (size_t)pointer);
}

void ParserGC_free(void) {
    if (!allocated_pointers) {
        fail_with_message("Attempting to deallocate non-initialized ParserGC");
    }
    garbage_collection_paused = true;
    struct PointerSet *freed_pointers = PointerSet_create();
    struct VoidPointerListElement *iterator = VoidPointerList_begin(allocated_pointers);
    while (iterator) {
        void *pointer = VoidPointerListIterator_get(iterator);
        if (!PointerSet_contains(marked_pointers, (size_t)pointer)) {
            if (PointerSet_contains(freed_pointers, (size_t)pointer)) {
                fail_with_message("Double free from ParserGC");
            }
            free(pointer);
            PointerSet_add(freed_pointers, (size_t)pointer);
        }
    }
    PointerSet_delete(freed_pointers);
    VoidPointerList_delete(allocated_pointers);
    PointerSet_delete(marked_pointers);
    allocated_pointers = 0;
    marked_pointers = 0;
    garbage_collection_paused = false;
}

static void store_pointer(void *value) {
    if (allocated_pointers && !garbage_collection_paused) {
        VoidPointerList_append(allocated_pointers, value);
    }
}

void * allocate(size_t size) {
    void *result = allocate_unmanaged(size);
    store_pointer(result);
    return result;
}

void * allocate_unmanaged(size_t size) {
    void *result = allocate_raw_unmanaged(size);
    memset(result, 0, size);
    return result;
}

void * allocate_raw(size_t size) {
    void *result = allocate_raw_unmanaged(size);
    store_pointer(result);
    return result;
}

void * allocate_raw_unmanaged(size_t size) {
    void *result = malloc(size);
    if (!result) {
        fail_with_message("Failed to allocate %zu bytes of memory.", size);
    }
    return result;
}

void deallocate(void *pointer) {
    if (allocated_pointers && !garbage_collection_paused) {
        return;
//        fail_with_message("free called with activated garbage collector");
    }
    free(pointer);
}

void deallocate_unmanaged(void *pointer) {
    free(pointer);
}
