#include "element.h"

#include <string.h>

#include <core/allocate.h>
#include <generated/token_queue.h>
#include <core/errors.h>

#include "generated/element_list.h"

const char *bracket_type_name(enum BracketType type) {
    switch (type) {
        case Paren:
            return "Paren";
        case Square:
            return "Square";
        case Curly:
            return "Curly";
        default:
            return "<unknown>";
    }
}

struct Elements *Elements_from_list(struct ElementList *elements) {
    struct Elements *result = allocate(sizeof(struct Elements));
    result->size = ElementList_size(elements);
    result->data = ElementList_to_array(elements);
    return result;
}

static bool equal(const struct String *string, const char *literal) {
    return String_equal_to_literal(string, literal);
}

static enum BracketType bracket_type(const struct Token *token) {
    const struct String *text = token->text;
    if (equal(text, "(") || equal(text, ")")) {
        return Paren;
    } else if (equal(text, "[") || equal(text, "]")) {
        return Square;
    } else if (equal(text, "{") || equal(text, "}")) {
        return Curly;
    } else {
        fail_at_position(token->position, "Not a bracket [%s]", text->value);
    }
}

struct Element *token_element(const struct Token *token) {
    struct Element *result = allocate(sizeof(struct Element));
    result->position = token->position;
    result->type = TokenElement;
    result->token = token;
    return result;
}

static struct ElementList *collect_elements(struct TokenQueue *tokens);

void add_bracket(struct ElementList *elements, struct TokenQueue *tokens, const struct Token *opening_bracket) {
    struct ElementList *bracket_elements = collect_elements(tokens);
    const struct Token *closing_bracket = TokenQueue_next(tokens);
    if (!closing_bracket) {
        fail_with_message("Missing closing bracket (opening bracket at line %zu, column %zu, file [%s])",
                          opening_bracket->position.line, opening_bracket->position.column,
                          opening_bracket->position.path);
    }
    enum BracketType type = bracket_type(opening_bracket);
    if (bracket_type(closing_bracket) != bracket_type(opening_bracket)) {
        fail_at_position(closing_bracket->position,
                         "Bracket type mismatch (opening bracket at line %zu, column %zu, file [%s])",
                         opening_bracket->position.line, opening_bracket->position.column,
                         opening_bracket->position.path);
    }
    struct Element *result = allocate(sizeof(struct Element));
    result->position = opening_bracket->position;
    result->type = BracketElement;
    result->bracket = allocate(sizeof(struct BracketElement));
    result->bracket->type = type;
    result->bracket->elements = Elements_from_list(bracket_elements);
    ElementList_delete(bracket_elements);
    result->bracket->opening_bracket = opening_bracket;
    result->bracket->closing_bracket = closing_bracket;
    ElementList_append(elements, result);
}

static void add_elements(struct ElementList *elements, struct TokenQueue *tokens) {
    while (true) {
        const struct Token *token = TokenQueue_peek(tokens);
        if (token) {
            if (token->type == OpeningBracketToken) {
                TokenQueue_next(tokens);
                add_bracket(elements, tokens, token);
            } else if (token->type == ClosingBracketToken) {
                return;
            } else {
                TokenQueue_next(tokens);
                ElementList_append(elements, token_element(token));
            }
        } else {
            return;
        }
    }
}

static struct ElementList *collect_elements(struct TokenQueue *tokens) {
    struct ElementList *result = ElementList_create();
    add_elements(result, tokens);
    return result;
}

struct Elements *read_elements(const struct Tokens *tokens) {
    struct TokenQueue *queue = TokenQueue_create(tokens);
    struct ElementList *elements = collect_elements(queue);
    const struct Token *next_element = TokenQueue_peek(queue);
    if (next_element) {
        fail_at_position(next_element->position,
                         "Unexpected token [%s] - too many closing brackets?",
                         next_element->text->value);
    }
    struct Elements *result = Elements_from_list(elements);
    ElementList_delete(elements);
    return result;
}
