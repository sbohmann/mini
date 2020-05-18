#include "voidpointer_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct VoidPointerList {
    size_t size;
    struct VoidPointerListElement *first;
    struct VoidPointerListElement *last;
};

struct VoidPointerListElement {
    void * value;
    struct VoidPointerListElement *next;
    struct VoidPointerListElement *previous;
};

struct VoidPointerList * VoidPointerList_create(void) {
    return allocate(sizeof(struct VoidPointerList));
}

void VoidPointerList_delete(struct VoidPointerList * instance) {
    struct VoidPointerListElement *element = instance->first;
    while (element) {
        struct VoidPointerListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t VoidPointerList_size(const struct VoidPointerList * self) {
    return self->size;
}

void VoidPointerList_append(struct VoidPointerList * self, void * value) {
    struct VoidPointerListElement * element = allocate(sizeof(struct VoidPointerListElement));
    element->value = value;
    if (self->size == 0) {
        self->first = element;
        self->last = element;
        self->size = 1;
    } else {
        self->last->next = element;
        element->previous = self->last;
        self->last = element;
        ++self->size;
    }
}

void VoidPointerList_prepend(struct VoidPointerList * self, void * value) {
    struct VoidPointerListElement * element = allocate(sizeof(struct VoidPointerListElement));
    element->value = value;
    if (self->size == 0) {
        self->first = element;
        self->last = element;
        self->size = 1;
    } else {
        self->first->previous = element;
        element->next = self->first;
        self->first = element;
        ++self->size;
    }
}

void * * VoidPointerList_to_array(const struct VoidPointerList * self) {
    const size_t size = VoidPointerList_size(self);
    const size_t element_size = sizeof(void *);
    const size_t result_size = element_size * size;
    void * *result = allocate(result_size);
    void * *result_iterator = result;
    struct VoidPointerListElement *iterator = VoidPointerList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail_with_message("Logical error in VoidPointerList_to_array - target overrun");
        }
        *result_iterator = VoidPointerListIterator_get(iterator);
        iterator = VoidPointerListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail_with_message("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct VoidPointerListElement * VoidPointerList_begin(const struct VoidPointerList * self) {
    return self->first;
}

struct VoidPointerListElement * VoidPointerList_end(const struct VoidPointerList * self) {
    return self->last;
}

struct VoidPointerListElement * VoidPointerListIterator_next(struct VoidPointerListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail_with_message("Attempt to get next from null iterator");
    }
}

struct VoidPointerListElement * VoidPointerListIterator_previous(struct VoidPointerListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail_with_message("Attempt to get previous from null iterator");
    }
}

void * VoidPointerListIterator_get(struct VoidPointerListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail_with_message("Attempt to get value from null iterator");
    }
}
