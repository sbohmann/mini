#include "token_list.h"

#include <stdlib.h>

#include "../allocate.h"
#include "../errors.h"

struct TokenList {
    size_t size;
    struct TokenListElement *first;
    struct TokenListElement *last;
};

struct TokenListElement {
    struct Token *value;
    struct TokenListElement *next;
    struct TokenListElement *previous;
};

struct TokenList * TokenList_create() {
    return allocate(sizeof(struct TokenList));
}

void TokenList_delete(struct TokenList * list) {
    struct TokenListElement *element = list->first;
    while (element) {
        struct TokenListElement *next = element->next;
        free(element);
        element = next;
    }
}

void TokenList_append(struct TokenList * list, struct Token *value) {
    struct TokenListElement * element = allocate(sizeof(struct TokenListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element;
        list->last = element;
        list->size = 1;
    } else {
        list->last->next = element;
        element->previous = list->last;
        list->last = element;
        ++list->size;
    }
}

void TokenList_prepend(struct TokenList * list, struct Token *value) {
    struct TokenListElement * element = allocate(sizeof(struct TokenListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element;
        list->last = element;
        list->size = 1;
    } else {
        list->first->previous = element;
        element->next = list->first;
        list->first = element;
        ++list->size;
    }
}

struct TokenListElement * TokenList_begin(struct TokenList * list) {
    return list->first;
}

struct TokenListElement * TokenList_end(struct TokenList * list) {
    return list->last;
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

struct Token * TokenListIterator_get(struct TokenListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
