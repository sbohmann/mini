#include <string.h>
#include <stdio.h>

#include <generated/element_queue.h>
#include <core/errors.h>
#include "micro.h"

const char * element_text(const struct Element *element) {
    if (element->type == TokenElement) {
        return element->token->text->value;
    } else if (element->type == BracketElement) {
        return element->bracket.opening_bracket->text->value;
    } else {
        return "<unknown element>";
    }
}

void read_comma(struct ElementQueue *arguments) {
    const struct Element *comma = ElementQueue_next(arguments);
    if (!comma) {
        fail("Unexpected end of input");
    }
    if (comma->type != TokenElement || comma->token->type != Operator || !equal(comma->token->text, ",")) {
        fail_at_position(comma->position, "Expected comma, found [%s]", element_text(comma));
    }
}

void print(struct ElementQueue *arguments) {
    bool first = true;
    while (true) {
        if (!ElementQueue_peek(arguments)) {
            break;
        }
        if (!first) {
            read_comma(arguments);
        }
        const struct Element *argument = ElementQueue_next(arguments);
        if (!argument) {
            break;
        }
        if (argument->type != TokenElement) {
            fail_at_position(argument->position, "Expected a value, found [%s]", element_text(argument));
        }
        if (argument->token->type == NumberLiteral) {
            if (argument->token->value.type == Integer) {
                printf("%d", (int) argument->token->value.integer);
            } else {
                puts("<a somewhat complicated number>");
            }
        } else if (argument->token->type == StringLiteral) {
            if (argument->token->value.type != String) {
                fail_at_position(argument->position, "Corrupt string token");
            }
            printf("%s", (int) argument->token->value.string->value);
        }
        first = false;
    }
}

void call(const struct String *name, const struct Elements *elements) {
    struct ElementQueue *queue = ElementQueue_create(elements);
    if (equal(name, "print")) {
        print(queue);
    }
}

void micro_run(struct ParsedModule *module) {
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (element->type != TokenElement || element->token->type != Symbol) {
            fail_at_position(element->position, "Expected symbol, found [%s]", element_text(element));
        }
        const struct Element *next = ElementQueue_next(queue);
        if (!next) {
            fail("Unepected end of input");
        }
        if (next->type != BracketElement) {
            fail_at_position(element->position, "Expected bracket expression, found [%s]", element_text(element));
        }
        call(element->token->text, &next->bracket.elements);
    }
    ElementQueue_delete(queue);
}

bool equal(const struct String *lhs, const char *rhs) {
    return strcmp(lhs->value, rhs) == 0;
}
