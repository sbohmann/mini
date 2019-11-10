#include "element.h"

#include <core/allocate.h>
#include <minic/tokens/token_queue.h>

#include "generated/element_list.c"

struct Element *token_element(const struct Token *token) {
    struct Element *result = allocate(sizeof(struct Element());
    result->type = TokenElement;
    result->token = token;
    return result;
}

void add_bracket(struct ElementList *elements, struct TokenQueue *tokens, const struct String *openingBracket) {
    struct ElementList *bracket_elements = collect_elements(queue);
    struct Elements *result = allocate(sizeof(struct Elements));
    return result;
}

void add_elements(struct ElementList *elements, struct TokenQueue *tokens) {
    while (true) {
        const struct Token *token = TokenQueue_peek(tokens);
        if (token) {
            if (token->type == OpeningBracket) {
                TokenQueue_next(tokens);
                add_bracket(elements, tokens, token->text);
            } else if (token->type == ClosingBracket) {
                return;
            } else {
                TokenQueue_next(tokens);
                ElementList_append(token_element(token));
            }
        } else {
            return;
        }
    }
}

struct ElementList *collect_elements(struct TokenQueue *tokens) {
    struct ElementList *result = ElementList_create();
    add_elements(result, tokens);
    return result;
}

struct Elements *read_elements(struct Tokens *tokens) {
    struct TokenQueue *queue = TokenQueue_create(tokens);
    struct ElementList *elements = collect_elements(queue);
    struct Elements *result = allocate(sizeof(struct Elements));
    result->data = allocate(sizeof(struct Element) * elements->size);
    result->size = 0;
    struct ElementListElement *iterator = ElementList_begin();
    while (iterator) {
        struct Element *element = ElementListIterator_get(iterator);
        if (result.size == elements->size) {
            fail("Overrun in flat element list creation");
        }
        result.data
    }
    if (result.size != elements->size) {
        fail("Underrun in flat element list creation");
    }
    ElementList_delete(elements);
    return result;
}
