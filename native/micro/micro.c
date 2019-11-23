#include "micro.h"

#include <core/errors.h>
#include <collections/hashmap.h>
#include <generated/element_queue.h>

#include <minic/expressions/expressions.h>
#include <generated/string_list.h>
#include <core/complex.h>
#include <core/allocate.h>

#include "debug.h"
#include "print.h"
#include "variables.h"

const uint32_t FunctionType = 0xd6dce275;

struct Function {
    struct ComplexValue base;
    uint32_t type;
    const struct StringList *parameter_names;
    const struct Elements *body;
};

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
    struct Any value = None();
    if (rhs->type == Symbol) {
        value = get_variable(rhs->text);
    } else if (rhs->type == NumberLiteral || rhs->type == StringLiteral) {
        value = rhs->value;
    } else {
        fail_at_position(rhs->position, "Unexpected expression: [%s]", rhs->text);
    };
    set_variable(variable_name, rhs->text, value);
}

static void fn(struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue);
    struct ElementQueue *parameters = ElementQueue_create(read_paren_block(queue));
    bool first = true;
    struct StringList *parameter_names = StringList_create();
    while (ElementQueue_peek(parameters)) {
        StringList_append(parameter_names, read_symbol(parameters));
        if (!first) {
            read_comma(parameters);
        }
        first = false;
    }
    const struct Elements *body = read_curly_block(queue);
    struct Function *function = allocate(sizeof(struct Function));
    function->type= FunctionType;
    function->parameter_names = parameter_names;
    function->body = body;
    static const struct String *text = 0;
    if (!text) {
        text = String_from_literal("function");
    }
    set_variable(name, text, Complex(&function->base));
}

static void call_function(struct Function *function, struct ElementQueue *arguments) {
    struct HashMap *locals = HashMap_create();
    struct StringListElement *name_iterator = StringList_begin(function->parameter_names);
//    while (true) {
//        struct String *name = StringListIterator_get(name_iterator);
//        read_
//    }
}

static void call(const struct String *name, struct ElementQueue *queue) {
    struct ElementQueue *arguments = ElementQueue_create(read_paren_block(queue));
    if (equal(name, "print")) {
        print(arguments);
    } else {
        struct Any value = get_variable(name);
        if (value.type == ComplexType) {
            struct Function *function = (struct Function *)value.complex_value;
            if (function->type == FunctionType) {
                call_function(function, queue);
            } else {
                printf("Error: failed to call non-function complex value %s\n", name->value);
            }
        } else if (value.type == NoneType) {
            printf("Error: failed to call unknown function %s\n", name->value);
        } else {
            printf("Error: failed to call non-function value %s [", name->value);
            print_value(value);
            puts("]\n");
        }
    }
    ElementQueue_delete(arguments);
}

void run_block(struct ElementQueue *queue) {
    while (ElementQueue_peek(queue)) {
        const struct String *symbol = read_symbol(queue);
        if (equal(symbol, "let")) {
            let(queue);
        } else if (equal(symbol, "fn")) {
            fn(queue);
        } else {
            call(symbol, queue);
        }
    }
}

void micro_run(struct ParsedModule *module) {
    initialize_variables();
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    run_block(queue);
    ElementQueue_delete(queue);
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
