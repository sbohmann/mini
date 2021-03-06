#include "expressions.h"

#include <stdbool.h>

#include <core/string.h>
#include <core/errors.h>
#include <minic/elements/element.h>

bool equal(const struct String *string, const char *literal) {
    return String_equal_to_literal(string, literal);
}

const char *element_text(const struct Element *element) {
    if (element->type == TokenElement) {
        return element->token->text->value;
    } else if (element->type == BracketElement) {
        return element->bracket->opening_bracket->text->value;
    } else {
        return "<unknown element>";
    }
}

void read_comma(struct ElementQueue *elements) {
    const struct Element *comma = ElementQueue_next(elements);
    if (!comma) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting comma");
    }
    if (comma->type != TokenElement || comma->token->type != OperatorToken || !equal(comma->token->text, ",")) {
        fail_at_position(comma->position, "Expected comma, found [%s]", element_text(comma));
    }
}

void read_operator(struct ElementQueue *elements) {
    const struct Element *operator = ElementQueue_next(elements);
    if (!operator) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting operator");
    }
    if (operator->type != TokenElement || operator->token->type != OperatorToken) {
        fail_at_position(operator->position, "Expected an operator, found [%s]", element_text(operator));
    }
}

void read_operator_with_text(struct ElementQueue *elements, const char *text) {
    const struct Element *operator = ElementQueue_next(elements);
    if (!operator) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting operator %s", text);
    }
    if (operator->type != TokenElement || operator->token->type != OperatorToken || !equal(operator->token->text, text)) {
        fail_at_position(operator->position, "Expected %s, found [%s]", text, element_text(operator));
    }
}

bool is_operator(const struct Element *candidate) {
    return candidate &&
           candidate->type == TokenElement &&
           candidate->token->type == OperatorToken;
}

bool is_operator_with_text(const struct Element *candidate, const char *text) {
    return is_operator(candidate) &&
           equal(candidate->token->text, text);
}

const struct Token *read_token(struct ElementQueue *elements) {
    const struct Element *next = ElementQueue_next(elements);
    if (!next) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting token");
    }
    if (next->type != TokenElement) {
        fail_at_position(next->position, "Expected expression, found [%s]", element_text(next));
    }
    return next->token;
}

const struct Token *read_symbol(struct ElementQueue *elements) {
    const struct Element *element = ElementQueue_next(elements);
    if (!element) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting symbol");
    }
    if (element->type != TokenElement || element->token->type != SymbolToken) {
        fail_at_position(element->position, "Expected symbol, found [%s]", element_text(element));
    }
    return element->token;
}

bool is_symbol(const struct Element *element) {
    return element &&
           element->type == TokenElement &&
           element->token->type == SymbolToken;
}

bool is_symbol_of_name(const struct Element *element, const char *name) {
    return is_symbol(element) &&
           equal(element->token->text, name);
}

static const struct Elements *read_block(struct ElementQueue *elements, enum BracketType type) {
    const struct Element *next = ElementQueue_next(elements);
    if (!next) {
        fail_after_position(ElementQueue_last_position(elements), "Unexpected end of input - expecting block after ");
    }
    if (next->type != BracketElement) {
        fail_at_position(next->position, "Expected bracket expression, found [%s]", element_text(next));
    }
    if (next->bracket->type != type) {
        fail_at_position(next->position, "Unexpected bracket type [%s], expected, [%s]",
                         bracket_type_name(next->bracket->type), bracket_type_name(type));
    }
    return next->bracket->elements;
}

const struct Elements *read_paren_block(struct ElementQueue *elements) {
    return read_block(elements, Paren);
}

const struct Elements *read_square_block(struct ElementQueue *elements) {
    return read_block(elements, Square);
}

const struct Elements *read_curly_block(struct ElementQueue *elements) {
    return read_block(elements, Curly);
}

bool is_bracket_element(const struct Element *element) {
    return element && element->type == BracketElement;
}

bool is_bracket_element_of_type(const struct Element *element, enum BracketType type) {
    return is_bracket_element(element) && element->bracket->type == type;
}
