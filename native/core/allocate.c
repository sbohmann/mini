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
        fail_with_message("Attempting to free non-initialized ParserGC");
    }
    garbage_collection_paused = true;
    struct VoidPointerListElement *iterator = VoidPointerList_begin(allocated_pointers);
    while (iterator) {
        void *pointer = VoidPointerListIterator_get(iterator);
        if (!PointerSet_contains(marked_pointers, (size_t)pointer)) {
            free(pointer);
        }
    }
    VoidPointerList_delete(allocated_pointers);
    PointerSet_delete(marked_pointers);
    allocated_pointers = 0;
    marked_pointers = 0;
    garbage_collection_paused = false;
}

void * allocate(size_t size) {
    void *result = allocate_unmanaged(size);
    if (allocated_pointers) {
        VoidPointerList_append(allocated_pointers, result);
    }
    return result;
}

void * allocate_unmanaged(size_t size) {
    void *result = allocate_raw_unmanaged(size);
    memset(result, 0, size);
    return result;
}

void * allocate_raw(size_t size) {
    void *result = allocate_raw_unmanaged(size);
    if (allocated_pointers) {
        VoidPointerList_append(allocated_pointers, result);
    }
    return result;
}

void * allocate_raw_unmanaged(size_t size) {
    void *result = malloc(size);
    if (!result) {
        fail_with_message("Failed to allocate %zu bytes of memory.", size);
    }
    return result;
}
