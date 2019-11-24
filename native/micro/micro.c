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

struct Variables *global_context;

struct Function {
    struct ComplexValue base;
    uint32_t type;
    struct Variables *bindings;
    const struct StringList *parameter_names;
    const struct Elements *body;
};

static void Function_destructor(struct Function *instance) {
    Variables_release(instance->bindings);
}

static struct Function *Function_create(struct Variables *bindings, const struct StringList *parameter_names, const struct Elements *body) {
    struct Function *result = allocate(sizeof(struct Function));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *)) Function_destructor;
    result->type = FunctionType;
    result->bindings = bindings;
    result->parameter_names = parameter_names;
    result->body = body;
    return result;
}

static struct Variables *create_bindings(struct Variables *context, const struct Elements *names) {
    struct ElementQueue *queue = ElementQueue_create(names);
    struct SplitElements *split_names = SplitElements_by_comma(queue);
    struct Variables *result = Variables_create(global_context);
    for (size_t index = 0; index < names->size; ++index) {
        struct Elements *part = &split_names->data[index];
        if (part->size == 0) {
            fail("logical_error");
        } else if (part->size > 1) {
            fail_at_position(part->data[1].position, "Unexpected token");
        }
        const struct Element *name_element = &part->data[0];
        if (name_element->type != TokenElement || name_element->token->type != Symbol) {
            fail_at_position(name_element->position, "Unexpected token");
        }
        const struct String *name = name_element->token->text;
        struct HashMapResult get_result = get_variable(context, name);
        if (!get_result.found) {
            fail_at_position(name_element->position, "Binding undefined variable %s", name->value);
        }
        create_variable(result, name, get_result.value);
    }
    ElementQueue_delete(queue);
    return result;
}

static void fn(struct Variables *context, struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue);
    struct Variables *bindings = 0;
    if (is_bracket_element_of_type(ElementQueue_peek(queue), Square)) {
        const struct Elements *bindings_block = read_square_block(queue);
        if (bindings_block->size > 0) {
            bindings = create_bindings(context, bindings_block);
        }
    }
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
    struct Function *function = Function_create(bindings, parameter_names, body);
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

static struct FunctionCallResult call(struct Variables *context, const struct String *name, struct Position position, struct ElementQueue *arguments);

struct Any evaluate_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Token *first_token = read_token(queue);
    struct Any result;
    if (first_token->type == Symbol) {
        if (ElementQueue_peek(queue)) {
            const struct Element *opening_bracket = ElementQueue_peek(queue);
            const struct Elements *arguments = read_paren_block(queue);
            struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
            struct FunctionCallResult call_result = call(context, first_token->text, opening_bracket->position, arguments_queue);
            ElementQueue_delete(arguments_queue);
            if (call_result.type == Success) {
                result = call_result.value;
            } else {
                fail_at_position(first_token->position, "Call to function %s failed.", first_token->text->value);
            }
        } else {
            struct HashMapResult variable = get_variable(context, first_token->text);
            if (variable.found) {
                result = variable.value;
            } else {
                fail_at_position(first_token->position, "Undefined variable [%s]", first_token->text->value);
            }
        }
    } else if (first_token->type == NumberLiteral || first_token->type == StringLiteral) {
        result = first_token->value;
    } else {
        fail_at_position(first_token->position, "Unexpected expression: [%s]", first_token->text->value);
    }
    const struct Element *next_element = ElementQueue_peek(queue);
    if (next_element) {
        fail_at_position(next_element->position, "Unexpected token");
    }
    return result;
}

void print(struct Variables *context, struct SplitElements *arguments) {
    for (size_t index = 0; index < arguments->size; ++index) {
        struct ElementQueue *argument_queue = ElementQueue_create(&arguments->data[index]);
        struct Any value = evaluate_expression(context, argument_queue);
        print_value(value);
    }
    fflush(stdout);
}

static struct FunctionCallResult call_function(struct Variables *context, struct Function *function, struct ElementQueue *arguments) {
    struct SplitElements *split_elements = SplitElements_by_comma(arguments);
    if (split_elements->size != StringList_size(function->parameter_names)) {
        struct FunctionCallResult result;
        result.type = ArgumentNumberMismatch;
        result.arguments_passed = split_elements->size;
        result.arguments_expected = StringList_size(function->parameter_names);
        return result;
    }
    struct Variables *locals = Variables_create(function->bindings ? function->bindings : global_context);
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

static struct FunctionCallResult call(struct Variables *context, const struct String *name, struct Position position, struct ElementQueue *arguments) {
    if (equal(name, "print")) {
        struct SplitElements *split_arguments = SplitElements_by_comma(arguments);
        print(context, split_arguments);
        struct FunctionCallResult result;
        result.type = Success;
        result.value = None();
        return result;
    } else {
        struct HashMapResult result = get_variable(context, name);
        if (!result.found) {
            fail_at_position(position, "Call to undefined value [%s]", name->value);
        }
        if (result.value.type == ComplexType) {
            struct Function *function = (struct Function *) result.value.complex_value;
            if (function->type == FunctionType) {
                struct FunctionCallResult call_result = call_function(context, function, arguments);
                if (call_result.type == ArgumentNumberMismatch) {
                    printf("Argument number mismatch in call to function %s - %zu arguments passed, %zu expected.",
                           name->value, call_result.arguments_passed, call_result.arguments_expected);
                }
                return call_result;
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
    fflush(stdout);
}

struct StatementResult {
    bool is_return;
    struct Any value;
};

struct StatementResult execute_statement(struct Variables *context, struct Elements *statement) {
    struct ElementQueue *queue = ElementQueue_create(statement);
    const struct String *symbol = read_symbol(queue);
    if (equal(symbol, "let")) {
        const struct String *name = read_symbol(queue);
        read_operator(queue, "=");
        create_variable(context, name, evaluate_expression(context, queue));
    } else if (equal(symbol, "fn")) {
        fn(context, queue);
    } else if (equal(symbol, "return")) {
        return (struct StatementResult) { true, evaluate_expression(context, queue) };
    } else {
        const struct Element *opening_bracket = ElementQueue_peek(queue);
        struct ElementQueue *arguments = ElementQueue_create(read_paren_block(queue));
        call(context, symbol, opening_bracket->position, arguments);
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
    global_context = globals;
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    run_block(globals, queue);
    ElementQueue_delete(queue);
    global_context = 0;
    Variables_release(globals);
}

int main() {
    struct ParsedModule *module = read_file("examples/print.micro");
    micro_run(module);
}
