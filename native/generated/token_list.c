#include "token_list.h"

#include "../allocate.h"

struct TokenList {
    size_t size;
    struct ListElement *first;
    struct ListElement *last;
};

struct TokenListElement {
    struct Token *value;
    TokenListElement *next;
    TokenListElement *previous;
};

TokenList * TokenList_create() {
    return allocate(sizeof(struct TokenList));
}

TokenList * TokenList_delete(struct TokenList * list) {
    TokenListElement *element = list->first;
    while (element) {
        TokenListElement *next = element->next;
        free(element);
        element = next;
    }
}

void TokenList_append(struct TokenList * list, struct Token *value) {
    TokenListElement * element = allocate(sizeof(TokenListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element
        list->last = element
        list->size = 1;
    } else {
        list->last->next = element;
        element->previous = list->last;
        list->last = element;
        ++list->size;
    }
}

void TokenList_prepend(struct TokenList * list, struct Token *value) {
    TokenListElement * element = allocate(sizeof(TokenListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element
        list->last = element
        list->size = 1;
    } else {
        list->first->previous = element;
        element->next = list->first;
        list->first = element;
        ++list->size;
    }
}

TokenListElement * TokenList_begin(struct TokenList * list) {
    return list->first;
}

TokenListElement * TokenList_end(struct TokenList * list) {
    return list->last;
}

TokenListElement * TokenListIterator_next(TokenListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

TokenListElement * TokenListIterator_previous(TokenListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

struct Token * TokenListIterator_get(TokenListElement * value) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}
