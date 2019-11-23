#include "micro.h"

#include <string.h>
#include <stdio.h>

#include <core/errors.h>
#include <collections/hashmap.h>
#include <generated/element_queue.h>

#include "minic/expressions/expressions.h"

#define DEBUG(format, ...) //printf((format), __VA_ARGS__)

struct HashMap *variables = 0;

void set_variable(const struct String *name, const struct String *text, const struct Any value) {
    DEBUG("Setting variable %s to [%s]\n", name->value, text->value);
    HashMap_put(variables, String(name), value);
}

struct Any get_variable(const struct String *name) {
    DEBUG("Getting variable %s\n", name->value);
    return HashMap_get(variables, String(name));
}

void let(struct ElementQueue *queue) {
    const struct Element *next = ElementQueue_next(queue);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != TokenElement || next->token->type != Symbol) {
        fail_at_position(next->position, "Expected a symbol, found [%s]", element_text(next));
    }
    const struct String *variable_name = next->token->text;
    read_operator(queue, "=");
    const struct Token *rhs = read_token(queue);
    struct Any value;
    if (rhs->type == Symbol) {
        value = get_variable(rhs->text);
    } else if (rhs->type == NumberLiteral || rhs->type == StringLiteral) {
        value = rhs->value;
    } else {
        fail_at_position(rhs->position, "Unexpected expression: [%s]", rhs->text);
    };
    set_variable(variable_name, rhs->text, value);
}

static void print(struct ElementQueue *arguments) {
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
        }
        first = false;
    }
    fflush(stdout);
}

static void call(const struct String *function, struct ElementQueue *queue) {
    struct ElementQueue *arguments = read_paren_block(queue);
    if (equal(function, "print")) {
        print(arguments);
    }
    ElementQueue_delete(arguments);
}

static void fn(struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue);
//    read_
}

void micro_run(struct ParsedModule *module) {
    variables = HashMap_create();
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    while (ElementQueue_peek(queue)) {
        const struct String *symbol = read_symbol(queue);
        if (equal(symbol, "let")) {
            let(queue);
        } else if (equal(symbol, "fn")) {
            fn(queue);
        }
        else {
            call(symbol, queue);
        }
    }
    ElementQueue_delete(queue);
    HashMap_delete(variables);
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
