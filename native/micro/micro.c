#include <string.h>
#include <stdio.h>

#include <generated/element_queue.h>
#include <core/errors.h>
#include <generated/variable_list.h>
#include <core/allocate.h>
#include "micro.h"

#define DEBUG(format, ...) //printf((format), __VA_ARGS__)

struct VariableList *variables = 0;

const char *element_text(const struct Element *element) {
    if (element->type == TokenElement) {
        return element->token->text->value;
    } else if (element->type == BracketElement) {
        return element->bracket.opening_bracket->text->value;
    } else {
        return "<unknown element>";
    }
}

void set_variable(const struct String *name, const struct String *text, const struct Any value) {
    DEBUG("Setting variable %s to [%s]\n", name->value, text->value);
    struct VariableListElement *iterator = VariableList_begin(variables);
    while (iterator) {
        struct Variable *variable = VariableListIterator_get(iterator);
        if (equals(variable->name, name)) {
            variable->value = value;
            return;
        }
    }
    struct Variable *variable = allocate(sizeof(struct Variable));
    variable->name = name;
    variable->value = value;
    VariableList_append(variables, variable);
}

struct Any get_variable(const struct String *name) {
    DEBUG("Getting variable %s\n", name->value);
    struct VariableListElement *iterator = VariableList_begin(variables);
    while (iterator) {
        struct Variable *variable = VariableListIterator_get(iterator);
        if (equals(variable->name, name)) {
            return variable->value;
        }
    }
    struct Any result;
    memset(&result, 0, sizeof(result));
    return result;
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

void let(struct ElementQueue *queue) {
    const struct Element *next = ElementQueue_next(queue);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != TokenElement || next->token->type != Symbol) {
        fail_at_position(next->position, "Expected a symbol, found [%s]", element_text(next));
    }
    const struct String *variable_name = next->token->text;
    next = ElementQueue_next(queue);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != TokenElement || next->token->type != Operator || !equal(next->token->text, "=")) {
        fail_at_position(next->position, "Expected '=', found [%s]", element_text(next));
    }
    next = ElementQueue_next(queue);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != TokenElement) {
        fail_at_position(next->position, "Expected expression, found [%s]", element_text(next));
    }
    struct Any value;
    if (next->token->type == Symbol) {
        value = get_variable(next->token->text);
    } else if (next->token->type == NumberLiteral || next->token->type == StringLiteral) {
        value = next->token->value;
    } else {
        fail_at_position(next->token->position, "Unexpected expression: [%s]", next->token->text);
    };
    set_variable(variable_name, next->token->text, value);
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
                    case UndefinedType:
                        printf("<undefined");
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
    const struct Element *next = ElementQueue_next(queue);
    if (!next) {
        fail("Unexpected end of input");
    }
    if (next->type != BracketElement) {
        fail_at_position(next->position, "Expected bracket expression, found [%s]", element_text(next));
    }
    struct ElementQueue *arguments = ElementQueue_create(&next->bracket.elements);
    if (equal(function, "print")) {
        print(arguments);
    }
}

void micro_run(struct ParsedModule *module) {
    variables = VariableList_create();
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (element->type != TokenElement || element->token->type != Symbol) {
            fail_at_position(element->position, "Expected symbol, found [%s]", element_text(element));
        }
        const struct String *symbol = element->token->text;
        if (equal(symbol, "let")) {
            let(queue);
        } else {
            call(symbol, queue);
        }
    }
    ElementQueue_delete(queue);
    VariableList_delete(variables);
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
