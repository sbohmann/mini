#include "int_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct IntList {
    size_t size;
    struct IntListElement *first;
    struct IntListElement *last;
};

struct IntListElement {
    int64_t value;
    struct IntListElement *next;
    struct IntListElement *previous;
};

struct IntList * IntList_create() {
    return allocate(sizeof(struct IntList));
}

struct IntList * IntList_delete(struct IntList * instance) {
    struct IntListElement *element = instance->first;
    while (element) {
        struct IntListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t IntList_size(struct IntList * self) {
    return self->size;
}

void IntList_append(struct IntList * self, int64_t value) {
    struct IntListElement * element = allocate(sizeof(struct IntListElement));
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

void IntList_prepend(struct IntList * self, int64_t value) {
    struct IntListElement * element = allocate(sizeof(struct IntListElement));
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

int64_t * IntList_to_array(struct IntList * self) {
    const size_t size = IntList_size(self);
    const size_t element_size = sizeof(int64_t);
    const size_t result_size = element_size * size;
    int64_t *result = allocate(result_size);
    int64_t *result_iterator = result;
    struct IntListElement *iterator = IntList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in IntList_to_array - target overrun");
        }
        *result_iterator = IntListIterator_get(iterator);
        iterator = IntListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct IntListElement * IntList_begin(struct IntList * self) {
    return self->first;
}

struct IntListElement * IntList_end(struct IntList * self) {
    return self->last;
}

struct IntListElement * IntListIterator_next(struct IntListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct IntListElement * IntListIterator_previous(struct IntListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

int64_t IntListIterator_get(struct IntListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
