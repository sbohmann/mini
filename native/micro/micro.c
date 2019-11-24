#include "micro.h"

#include <core/errors.h>
#include <collections/hashmap.h>
#include <generated/element_queue.h>

#include <minic/expressions/expressions.h>
#include <generated/string_list.h>
#include <core/complex.h>
#include <core/allocate.h>
#include <stdlib.h>

#include "debug.h"
#include "print.h"
#include "variables.h"
#include "split_elements.h"

const uint32_t FunctionType = 0xd6dce275;

struct Function {
    struct ComplexValue base;
    uint32_t type;
    const struct StringList *parameter_names;
    const struct Elements *body;
};

void let(struct Variables *context, struct ElementQueue *queue) {
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
        struct HashMapResult result = get_variable(context, rhs->text);
        if (result.found) {
            // TODO allow calls by using peek
            value = result.value;
        } else {
            fail_at_position(rhs->position, "Undefined variable: [%s]", rhs->text);
        }
    } else if (rhs->type == NumberLiteral || rhs->type == StringLiteral) {
        value = rhs->value;
    } else {
        fail_at_position(rhs->position, "Unexpected expression: [%s]", rhs->text);
    }
    create_variable(context, variable_name, value);
}

static void fn(struct Variables *context, struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue);
    struct ElementQueue *parameters = ElementQueue_create(read_paren_block(queue));
    bool first = true;
    struct StringList *parameter_names = StringList_create();
    while (ElementQueue_peek(parameters)) {
        if (!first) {
            read_comma(parameters);
        }
        StringList_append(parameter_names, read_symbol(parameters));
        first = false;
    }
    ElementQueue_delete(parameters);
    const struct Elements *body = read_curly_block(queue);
    struct Function *function = allocate(sizeof(struct Function));
    Complex_init(&function->base);
    function->type = FunctionType;
    function->parameter_names = parameter_names;
    function->body = body;
    static const struct String *text = 0;
    if (!text) {
        text = String_from_literal("function");
    }
    create_variable(context, name, Complex(&function->base));
}

struct Any run_block(struct Variables *context, struct ElementQueue *queue);

enum FunctionCallResultType {
    Success,
    Error,
    ArgumentNumberMismatch
};

struct FunctionCallResult {
    enum FunctionCallResultType type;
    union {
        struct Any value;
        struct {
            size_t arguments_passed;
            size_t arguments_expected;
        };
    };
};

static struct FunctionCallResult call(struct Variables *context, const struct String *name, struct ElementQueue *queue);

struct Any evaluate_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Token *first_token = read_token(queue);
    struct Any result;
    if (first_token->type == Symbol) {
        if (ElementQueue_peek(queue)) {
            const struct Elements *arguments = read_paren_block(queue);
            struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
            struct FunctionCallResult call_result = call(context, first_token->text, arguments_queue);
            ElementQueue_delete(arguments_queue);
            if (call_result.type == Success) {
                result = call_result.value;
            } else {
                fail_at_position(first_token->position, "Call to function %s failed.", first_token->text);
            }
        } else {
            struct HashMapResult variable = get_variable(context, first_token->text);
            if (variable.found) {
                result = variable.value;
            } else {
                fail_at_position(first_token->position, "Undefined variable [%s]", first_token->text);
            }
        }
    } else if (first_token->type == NumberLiteral || first_token->type == StringLiteral) {
        result = first_token->value;
    } else {
        fail_at_position(first_token->position, "Unexpected expression: [%s]", first_token->text);
    }
    const struct Element *next_element = ElementQueue_peek(queue);
    if (next_element) {
        fail_at_position(next_element->position, "Unexpected token");
    }
    return result;
}

static struct FunctionCallResult
call_function(struct Variables *context, struct Function *function, struct ElementQueue *arguments) {
    struct SplitElements *split_elements = SplitElements_by_comma(arguments);
    if (split_elements->size != StringList_size(function->parameter_names)) {
        struct FunctionCallResult result;
        result.type = ArgumentNumberMismatch;
        result.arguments_passed = split_elements->size;
        result.arguments_expected = StringList_size(function->parameter_names);
        return result;
    }
    struct Variables *locals = Variables_create(context);
    struct StringListElement *name_iterator = StringList_begin(function->parameter_names);
    size_t index = 0;
    while (name_iterator) {
        const struct String *name = StringListIterator_get(name_iterator);
        if (index >= split_elements->size) {
            fail("Logical error");
        }
        struct Elements *expression = &split_elements->data[index];
        struct ElementQueue *expression_queue = ElementQueue_create(expression);
        struct Any value = evaluate_expression(context, expression_queue);
        ElementQueue_delete(expression_queue);
        create_variable(locals, name, value);
        name_iterator = StringListIterator_next(name_iterator);
        ++index;
    }
    if (index != split_elements->size) {
        fail("Logical error");
    }
    SplitElements_delete(split_elements);
    // TODO split by line
    // evaluate_expression(function->body);
    struct ElementQueue *body_queue = ElementQueue_create(function->body);
    struct Any function_result = run_block(locals, body_queue);
    ElementQueue_delete(body_queue);
    Variables_release(locals);
    // TODO determine function return value
    struct FunctionCallResult result;
    result.type = Success;
    result.value = function_result;
    return result;
}

static struct FunctionCallResult
call(struct Variables *context, const struct String *name, struct ElementQueue *queue) {
    const struct Element *opening_bracket = ElementQueue_peek(queue);
    struct ElementQueue *arguments = ElementQueue_create(read_paren_block(queue));
    if (equal(name, "print")) {
        print(context, arguments);
        struct FunctionCallResult result;
        result.type = Success;
        result.value = None();
        return result;
    } else {
        struct HashMapResult result = get_variable(context, name);
        if (!result.found) {
            fail_at_position(opening_bracket->position, "Call to undefined value [%s]", name->value);
        }
        if (result.value.type == ComplexType) {
            struct Function *function = (struct Function *) result.value.complex_value;
            if (function->type == FunctionType) {
                struct FunctionCallResult result = call_function(context, function, arguments);
                if (result.type == ArgumentNumberMismatch) {
                    printf("Argument number mismatch in call to function %s - %zu arguments passed, %zu expected.",
                           name->value, result.arguments_passed, result.arguments_expected);
                }
                return result;
            } else {
                printf("Error: failed to call non-function complex value %s\n", name->value);
            }
        } else {
            printf("Error: failed to call non-function value %s [", name->value);
            print_value(result.value);
            puts("]\n");
        }
    }
    ElementQueue_delete(arguments);
    struct FunctionCallResult result;
    result.type = Error;
    return result;
}

void print_statement(struct Elements *statement) {
    printf("Executing statement [");
    for (size_t index = 0; index < statement->size; ++index) {
        if (index > 0) {
            putchar(' ');
        }
        struct Element element = statement->data[index];
        if (element.type == TokenElement) {
            printf("%s", element.token->text->value);
        } else if (element.type == BracketElement) {
            printf("%s", bracket_type_name(element.bracket.type));
        } else {
            printf("???");
        }
    }
    puts("]");
}

struct StatementResult {
    bool is_return;
    struct Any value;
};

struct StatementResult execute_statement(struct Variables *context, struct Elements *statement) {
    struct ElementQueue *queue = ElementQueue_create(statement);
    const struct String *symbol = read_symbol(queue);
    if (equal(symbol, "let")) {
        let(context, queue);
    } else if (equal(symbol, "fn")) {
        fn(context, queue);
    } else if (equal(symbol, "return")) {
        return (struct StatementResult) { true, evaluate_expression(context, queue) };
    } else {
        call(context, symbol, queue);
    }
    return (struct StatementResult) { false, None() };
}

struct Any run_block(struct Variables *context, struct ElementQueue *queue) {
    struct Any result = None();
    struct SplitElements *statements = SplitElements_by_line(queue);
    for (size_t index = 0; index < statements->size; ++index) {
        struct Elements *statement = &statements->data[index];
        if (DEBUG_ENABLED) {
            print_statement(statement);
        }
        struct StatementResult statement_result = execute_statement(context, statement);
        if (statement_result.is_return) {
            result = statement_result.value;
            break;
        }
    }
    SplitElements_delete(statements);
    return result;
}

void micro_run(struct ParsedModule *module) {
    struct Variables *globals = Variables_create(0);
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    run_block(globals, queue);
    ElementQueue_delete(queue);
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
