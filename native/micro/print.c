#include "print.h"

#include <stdio.h>
#include <string.h>

#include <collections/hashmap.h>
#include <minic/expressions/expressions.h>

#include "variables.h"
#include "debug.h"

void print_value(struct Any value) {
    switch (value.type) {
        case NoneType:
            printf("None");
            break;
        case IntegerType:
            printf("%d", (int) value.integer);
            break;
        case StringType:
            printf("%s", value.string->value);
            break;
        case ComplexType:
            // TODO
            printf("<complex>");
            break;
        case FlatType:
            printf("[");
            for (size_t index = 0; index < 8; ++index) {
                printf("%s%02x", (index > 0 ? " " : ""), (uint8_t) value.flat_value[index]);
            }
            printf("]");
            break;
        default:
            printf("<unknown>");
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
            if (argument->token->value.type == IntegerType) {
                printf("%d", (int) argument->token->value.integer);
            } else {
                puts("<a somewhat complicated number>");
            }
        } else if (argument->token->type == StringLiteral) {
            if (argument->token->value.type != StringType) {
                fail_at_position(argument->position, "Corrupt string token");
            }
            printf("%s", argument->token->value.string->value);
        } else if (argument->token->type == Symbol) {
            const struct Element *next = ElementQueue_peek(arguments);
            if (next && next->type == BracketElement) {
                // TODO print result of call
                printf("<call to %s>", argument->token->text->value);
            } else {
                struct Any value = get_variable(argument->token->text);
                print_value(value);
            }
        }
        first = false;
    }
    fflush(stdout);
}