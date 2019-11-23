#include "element_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct ElementList {
    size_t size;
    struct ElementListElement *first;
    struct ElementListElement *last;
};

struct ElementListElement {
    const struct Element *value;
    struct ElementListElement *next;
    struct ElementListElement *previous;
};

struct ElementList * ElementList_create() {
    return allocate(sizeof(struct ElementList));
}

void ElementList_delete(struct ElementList * instance) {
    struct ElementListElement *element = instance->first;
    while (element) {
        struct ElementListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t ElementList_size(const struct ElementList * self) {
    return self->size;
}

void ElementList_append(struct ElementList * self, const struct Element *value) {
    struct ElementListElement * element = allocate(sizeof(struct ElementListElement));
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

void ElementList_prepend(struct ElementList * self, const struct Element *value) {
    struct ElementListElement * element = allocate(sizeof(struct ElementListElement));
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

struct Element * ElementList_to_array(const struct ElementList * self) {
    const size_t size = ElementList_size(self);
    const size_t element_size = sizeof(struct Element);
    const size_t result_size = element_size * size;
    struct Element *result = allocate(result_size);
    struct Element *result_iterator = result;
    struct ElementListElement *iterator = ElementList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in ElementList_to_array - target overrun");
        }
        *result_iterator = *ElementListIterator_get(iterator);
        iterator = ElementListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct ElementListElement * ElementList_begin(const struct ElementList * self) {
    return self->first;
}

struct ElementListElement * ElementList_end(const struct ElementList * self) {
    return self->last;
}

struct ElementListElement * ElementListIterator_next(struct ElementListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct ElementListElement * ElementListIterator_previous(struct ElementListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

const struct Element * ElementListIterator_get(struct ElementListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
