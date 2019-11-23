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
        return element->bracket.opening_bracket->text->value;
    } else {
        return "<unknown element>";
    }
}

void read_comma(struct ElementQueue *elements) {
    const struct Element *comma = ElementQueue_next(elements);
    if (!comma) {
        fail("Unexpected end of input");
    }
    if (comma->type != TokenElement || comma->token->type != Operator || !equal(comma->token->text, ",")) {
        fail_at_position(comma->position, "Expected comma, found [%s]", element_text(comma));
    }
}

void read_operator(struct ElementQueue *elements, const char *text) {
    const struct Element *comma = ElementQueue_next(elements);
    if (!comma) {
        fail("Unexpected end of input");
    }
    if (comma->type != TokenElement || comma->token->type != Operator || !equal(comma->token->text, text)) {
        fail_at_position(comma->position, "Expected %s, found [%s]", text, element_text(comma));
    }
}

const struct Token *read_token(struct ElementQueue *elements) {
    const struct Element *next = ElementQueue_next(elements);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != TokenElement) {
        fail_at_position(next->position, "Expected expression, found [%s]", element_text(next));
    }
    return next->token;
}

const struct String *read_symbol(struct ElementQueue *elements) {
    const struct Element *element = ElementQueue_next(elements);
    if (!element) {
        fail("Unexpected end of input");
    }
    if (element->type != TokenElement || element->token->type != Symbol) {
        fail_at_position(element->position, "Expected symbol, found [%s]", element_text(element));
    }
    return element->token->text;
}

static const struct Elements *read_block(struct ElementQueue *elements, enum BracketType type) {
    const struct Element *next = ElementQueue_next(elements);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != BracketElement) {
        fail_at_position(next->position, "Expected bracket expression, found [%s]", element_text(next));
    }
    
    if (next->bracket.type != type) {
        fail_at_position(next->position, "Unexpected bracket type [%s], expected, [%s]",
                bracket_type_name(next->bracket.type), bracket_type_name(type));
    }
    return next->bracket.elements;
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
