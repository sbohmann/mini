#include "variable_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct VariableList {
    size_t size;
    struct VariableListElement *first;
    struct VariableListElement *last;
};

struct VariableListElement {
    struct Variable *value;
    struct VariableListElement *next;
    struct VariableListElement *previous;
};

struct VariableList * VariableList_create() {
    return allocate(sizeof(struct VariableList));
}

void VariableList_delete(struct VariableList * instance) {
    struct VariableListElement *element = instance->first;
    while (element) {
        struct VariableListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t VariableList_size(struct VariableList * self) {
    return self->size;
}

void VariableList_append(struct VariableList * self, struct Variable *value) {
    struct VariableListElement * element = allocate(sizeof(struct VariableListElement));
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

void VariableList_prepend(struct VariableList * self, struct Variable *value) {
    struct VariableListElement * element = allocate(sizeof(struct VariableListElement));
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

struct Variable * VariableList_to_array(struct VariableList * self) {
    const size_t size = VariableList_size(self);
    const size_t element_size = sizeof(struct Variable);
    const size_t result_size = element_size * size;
    struct Variable *result = allocate(result_size);
    struct Variable *result_iterator = result;
    struct VariableListElement *iterator = VariableList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in VariableList_to_array - target overrun");
        }
        *result_iterator = *VariableListIterator_get(iterator);
        iterator = VariableListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct VariableListElement * VariableList_begin(struct VariableList * self) {
    return self->first;
}

struct VariableListElement * VariableList_end(struct VariableList * self) {
    return self->last;
}

struct VariableListElement * VariableListIterator_next(struct VariableListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct VariableListElement * VariableListIterator_previous(struct VariableListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

struct Variable * VariableListIterator_get(struct VariableListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
