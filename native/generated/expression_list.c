#include "expression_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct ExpressionList {
    size_t size;
    struct ExpressionListElement *first;
    struct ExpressionListElement *last;
};

struct ExpressionListElement {
    const struct Expression *value;
    struct ExpressionListElement *next;
    struct ExpressionListElement *previous;
};

struct ExpressionList * ExpressionList_create(void) {
    return allocate(sizeof(struct ExpressionList));
}

void ExpressionList_delete(struct ExpressionList * instance) {
    struct ExpressionListElement *element = instance->first;
    while (element) {
        struct ExpressionListElement *next = element->next;
        deallocate(element);
        element = next;
    }
    deallocate(instance);
}

size_t ExpressionList_size(const struct ExpressionList * self) {
    return self->size;
}

void ExpressionList_append(struct ExpressionList * self, const struct Expression *value) {
    struct ExpressionListElement * element = allocate(sizeof(struct ExpressionListElement));
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

void ExpressionList_prepend(struct ExpressionList * self, const struct Expression *value) {
    struct ExpressionListElement * element = allocate(sizeof(struct ExpressionListElement));
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

struct Expression * ExpressionList_to_array(const struct ExpressionList * self) {
    const size_t size = ExpressionList_size(self);
    const size_t element_size = sizeof(struct Expression);
    const size_t result_size = element_size * size;
    struct Expression *result = allocate(result_size);
    struct Expression *result_iterator = result;
    struct ExpressionListElement *iterator = ExpressionList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail_with_message("Logical error in ExpressionList_to_array - target overrun");
        }
        *result_iterator = *ExpressionListIterator_get(iterator);
        iterator = ExpressionListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail_with_message("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct ExpressionListElement * ExpressionList_begin(const struct ExpressionList * self) {
    return self->first;
}

struct ExpressionListElement * ExpressionList_end(const struct ExpressionList * self) {
    return self->last;
}

struct ExpressionListElement * ExpressionListIterator_next(struct ExpressionListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail_with_message("Attempt to get next from null iterator");
    }
}

struct ExpressionListElement * ExpressionListIterator_previous(struct ExpressionListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail_with_message("Attempt to get previous from null iterator");
    }
}

const struct Expression * ExpressionListIterator_get(struct ExpressionListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail_with_message("Attempt to get value from null iterator");
    }
}
