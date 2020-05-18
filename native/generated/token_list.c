#include "token_list.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

struct TokenList {
    size_t size;
    struct TokenListElement *first;
    struct TokenListElement *last;
};

struct TokenListElement {
    const struct Token *value;
    struct TokenListElement *next;
    struct TokenListElement *previous;
};

struct TokenList * TokenList_create() {
    return allocate(sizeof(struct TokenList));
}

void TokenList_delete(struct TokenList * instance) {
    struct TokenListElement *element = instance->first;
    while (element) {
        struct TokenListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t TokenList_size(const struct TokenList * self) {
    return self->size;
}

void TokenList_append(struct TokenList * self, const struct Token *value) {
    struct TokenListElement * element = allocate(sizeof(struct TokenListElement));
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

void TokenList_prepend(struct TokenList * self, const struct Token *value) {
    struct TokenListElement * element = allocate(sizeof(struct TokenListElement));
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

struct Token * TokenList_to_array(const struct TokenList * self) {
    const size_t size = TokenList_size(self);
    const size_t element_size = sizeof(struct Token);
    const size_t result_size = element_size * size;
    struct Token *result = allocate(result_size);
    struct Token *result_iterator = result;
    struct TokenListElement *iterator = TokenList_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in TokenList_to_array - target overrun");
        }
        *result_iterator = *TokenListIterator_get(iterator);
        iterator = TokenListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct TokenListElement * TokenList_begin(const struct TokenList * self) {
    return self->first;
}

struct TokenListElement * TokenList_end(const struct TokenList * self) {
    return self->last;
}

struct TokenListElement * TokenListIterator_next(struct TokenListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct TokenListElement * TokenListIterator_previous(struct TokenListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

const struct Token * TokenListIterator_get(struct TokenListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
