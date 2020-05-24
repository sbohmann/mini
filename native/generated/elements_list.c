#include "elements_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct ElementsList {
    size_t size;
    struct ElementsListElement *first;
    struct ElementsListElement *last;
};

struct ElementsListElement {
    const struct Elements *value;
    struct ElementsListElement *next;
    struct ElementsListElement *previous;
};

struct ElementsList * ElementsList_create(void) {
    return allocate(sizeof(struct ElementsList));
}

void ElementsList_delete(struct ElementsList * instance) {
    struct ElementsListElement *element = instance->first;
    while (element) {
        struct ElementsListElement *next = element->next;
        deallocate(element);
        element = next;
    }
    deallocate(instance);
}

size_t ElementsList_size(const struct ElementsList * self) {
    return self->size;
}

void ElementsList_append(struct ElementsList * self, const struct Elements *value) {
    struct ElementsListElement * element = allocate(sizeof(struct ElementsListElement));
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

void ElementsList_prepend(struct ElementsList * self, const struct Elements *value) {
    struct ElementsListElement * element = allocate(sizeof(struct ElementsListElement));
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

struct Elements * ElementsList_to_array(const struct ElementsList * self) {
    const size_t size = ElementsList_size(self);
    const size_t element_size = sizeof(struct Elements);
    const size_t result_size = element_size * size;
    struct Elements *result = allocate(result_size);
    struct Elements *result_iterator = result;
    struct ElementsListElement *iterator = ElementsList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail_with_message("Logical error in ElementsList_to_array - target overrun");
        }
        *result_iterator = *ElementsListIterator_get(iterator);
        iterator = ElementsListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail_with_message("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct ElementsListElement * ElementsList_begin(const struct ElementsList * self) {
    return self->first;
}

struct ElementsListElement * ElementsList_end(const struct ElementsList * self) {
    return self->last;
}

struct ElementsListElement * ElementsListIterator_next(struct ElementsListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail_with_message("Attempt to get next from null iterator");
    }
}

struct ElementsListElement * ElementsListIterator_previous(struct ElementsListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail_with_message("Attempt to get previous from null iterator");
    }
}

const struct Elements * ElementsListIterator_get(struct ElementsListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail_with_message("Attempt to get value from null iterator");
    }
}
