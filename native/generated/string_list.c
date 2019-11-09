#include "string_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct StringList {
    size_t size;
    struct StringListElement *first;
    struct StringListElement *last;
};

struct StringListElement {
    struct String *value;
    struct StringListElement *next;
    struct StringListElement *previous;
};

struct StringList * StringList_create() {
    return allocate(sizeof(struct StringList));
}

struct StringList * StringList_delete(struct StringList * instance) {
    struct StringListElement *element = instance->first;
    while (element) {
        struct StringListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t StringList_size(struct StringList * self) {
    return self->size;
}

void StringList_append(struct StringList * self, struct String *value) {
    struct StringListElement * element = allocate(sizeof(struct StringListElement));
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

void StringList_prepend(struct StringList * self, struct String *value) {
    struct StringListElement * element = allocate(sizeof(struct StringListElement));
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

struct String * StringList_to_array(struct StringList * self) {
    const size_t size = StringList_size(self);
    const size_t element_size = sizeof(struct String);
    const size_t result_size = element_size * size;
    struct String *result = allocate(result_size);
    struct String *result_iterator = result;
    struct StringListElement *iterator = StringList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in StringList_to_array - target overrun");
        }
        *result_iterator = *StringListIterator_get(iterator);
        iterator = StringListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct StringListElement * StringList_begin(struct StringList * self) {
    return self->first;
}

struct StringListElement * StringList_end(struct StringList * self) {
    return self->last;
}

struct StringListElement * StringListIterator_next(struct StringListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct StringListElement * StringListIterator_previous(struct StringListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

struct String * StringListIterator_get(struct StringListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}