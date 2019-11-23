#include "micro.h"

#include <core/errors.h>
#include <collections/hashmap.h>
#include <generated/element_queue.h>

#include <minic/expressions/expressions.h>

#include "debug.h"
#include "print.h"
#include "variables.h"

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
    initialize_variables();
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
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
