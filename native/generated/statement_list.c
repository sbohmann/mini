#include "statement_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct StatementList {
    size_t size;
    struct StatementListElement *first;
    struct StatementListElement *last;
};

struct StatementListElement {
    const struct Statement *value;
    struct StatementListElement *next;
    struct StatementListElement *previous;
};

struct StatementList * StatementList_create(void) {
    return allocate(sizeof(struct StatementList));
}

void StatementList_delete(struct StatementList * instance) {
    struct StatementListElement *element = instance->first;
    while (element) {
        struct StatementListElement *next = element->next;
        deallocate(element);
        element = next;
    }
    deallocate(instance);
}

size_t StatementList_size(const struct StatementList * self) {
    return self->size;
}

void StatementList_append(struct StatementList * self, const struct Statement *value) {
    struct StatementListElement * element = allocate(sizeof(struct StatementListElement));
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

void StatementList_prepend(struct StatementList * self, const struct Statement *value) {
    struct StatementListElement * element = allocate(sizeof(struct StatementListElement));
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

struct Statement * StatementList_to_array(const struct StatementList * self) {
    const size_t size = StatementList_size(self);
    const size_t element_size = sizeof(struct Statement);
    const size_t result_size = element_size * size;
    struct Statement *result = allocate(result_size);
    struct Statement *result_iterator = result;
    struct StatementListElement *iterator = StatementList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail_with_message("Logical error in StatementList_to_array - target overrun");
        }
        *result_iterator = *StatementListIterator_get(iterator);
        iterator = StatementListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail_with_message("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct StatementListElement * StatementList_begin(const struct StatementList * self) {
    return self->first;
}

struct StatementListElement * StatementList_end(const struct StatementList * self) {
    return self->last;
}

struct StatementListElement * StatementListIterator_next(struct StatementListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail_with_message("Attempt to get next from null iterator");
    }
}

struct StatementListElement * StatementListIterator_previous(struct StatementListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail_with_message("Attempt to get previous from null iterator");
    }
}

const struct Statement * StatementListIterator_get(struct StatementListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail_with_message("Attempt to get value from null iterator");
    }
}
