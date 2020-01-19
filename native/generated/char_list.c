#include "char_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct CharList {
    size_t size;
    struct CharListElement *first;
    struct CharListElement *last;
};

struct CharListElement {
    char value;
    struct CharListElement *next;
    struct CharListElement *previous;
};

struct CharList * CharList_create() {
    return allocate(sizeof(struct CharList));
}

void CharList_delete(struct CharList * instance) {
    struct CharListElement *element = instance->first;
    while (element) {
        struct CharListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t CharList_size(const struct CharList * self) {
    return self->size;
}

void CharList_append(struct CharList * self, char value) {
    struct CharListElement * element = allocate(sizeof(struct CharListElement));
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

void CharList_prepend(struct CharList * self, char value) {
    struct CharListElement * element = allocate(sizeof(struct CharListElement));
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

char * CharList_to_array(const struct CharList * self) {
    const size_t size = CharList_size(self);
    const size_t element_size = sizeof(char);
    const size_t result_size = element_size * size;
    char *result = allocate(result_size);
    char *result_iterator = result;
    struct CharListElement *iterator = CharList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail_with_message("Logical error in CharList_to_array - target overrun");
        }
        *result_iterator = CharListIterator_get(iterator);
        iterator = CharListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail_with_message("Logical error in result lines creation - offset: [%zu], size: [%zu]",
                          result_iterator - result, size);
    }
    return result;
}

struct CharListElement * CharList_begin(const struct CharList * self) {
    return self->first;
}

struct CharListElement * CharList_end(const struct CharList * self) {
    return self->last;
}

struct CharListElement * CharListIterator_next(struct CharListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail_with_message("Attempt to get next from null iterator");
    }
}

struct CharListElement * CharListIterator_previous(struct CharListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail_with_message("Attempt to get previous from null iterator");
    }
}

char CharListIterator_get(struct CharListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail_with_message("Attempt to get value from null iterator");
    }
}
