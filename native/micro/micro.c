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
    if (instance->bindings) {
        Variables_release(instance->bindings);
    }
}

static struct Function *Function_create(struct Variables *bindings, const struct StringList *parameter_names,
                                        const struct Elements *body) {
    struct Function *result = allocate(sizeof(struct Function));
    Complex_init(&result->base);
    result->base.destructor = (void (*)(struct ComplexValue *)) Function_destructor;
    result->type = FunctionType;
    result->bindings = bindings;
    result->parameter_names = parameter_names;
    result->body = body;
    return result;
}

static struct Variables *create_bindings(const struct Variables *context, const struct Elements *names) {
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

struct Function *read_function(const struct Variables *context, struct ElementQueue *queue) {
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
        StringList_append(parameter_names, read_symbol(parameters)->text);
        first = false;
    }
    ElementQueue_delete(parameters);
    const struct Elements *body = read_curly_block(queue);
    return Function_create(bindings, parameter_names, body);
}

static void fn(struct Variables *context, struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue)->text;
    struct Function *function = read_function(context, queue);
    create_variable(context, name, Complex(&function->base));
}

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

static struct FunctionCallResult call(struct Variables *context, const struct String *name, struct Position position,
                                      struct ElementQueue *arguments);

struct Any with_queue(struct Variables *context, const struct Elements *elements,
                      struct Any (*function)(struct Variables *context, struct ElementQueue *queue)) {
    struct ElementQueue *queue = ElementQueue_create(elements);
    struct Any result = function(context, queue);
    ElementQueue_delete(queue);
    return result;
}

struct Any evaluate_simple_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Token *first_token = read_token(queue);
    struct Any result;
    const struct Element *next_element = ElementQueue_peek(queue);
    if (first_token->type == Symbol) {
        if (equal(first_token->text, "fn")) {
            struct Function *function = read_function(context, queue);
            return Complex(&function->base);
        } else if (is_bracket_element_of_type(next_element, Paren)) {
            const struct Element *opening_bracket = next_element;
            const struct Elements *arguments = read_paren_block(queue);
            struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
            struct FunctionCallResult call_result = call(context, first_token->text, opening_bracket->position,
                                                         arguments_queue);
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
    const struct Element *first_remaining_element = ElementQueue_peek(queue);
    if (first_remaining_element) {
        fail_at_position(first_remaining_element->position, "Unexpected token");
    }
    return result;
}

bool is_division_operator(const struct Element *element) {
    return is_operator_with_text(element, "/");
}

struct Any evaluate_division(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_division_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_division_operator);
        struct Any result = None();
        for (size_t index = 0; index < split_elements->size; ++index) {
            struct Elements *group = &split_elements->data[index];
            struct Any group_result = with_queue(context, group, evaluate_simple_expression);
            if (index == 0) {
                result = group_result;
            } else {
                result = Any_divide(result, group_result);
            }
        }
        SplitElements_delete(split_elements);
        return result;
    } else {
        return evaluate_simple_expression(context, queue);
    }
}

bool is_multiplication_operator(const struct Element *element) {
    return is_operator_with_text(element, "*");
}

struct Any evaluate_multiplication(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_multiplication_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_multiplication_operator);
        struct Any result = None();
        for (size_t index = 0; index < split_elements->size; ++index) {
            struct Elements *group = &split_elements->data[index];
            struct Any group_result = with_queue(context, group, evaluate_division);
            if (index == 0) {
                result = group_result;
            } else {
                result = Any_multiply(result, group_result);
            }
        }
        SplitElements_delete(split_elements);
        return result;
    } else {
        return evaluate_division(context, queue);
    }
}

bool is_minus_operator(const struct Element *element) {
    return is_operator_with_text(element, "-");
}

struct Any evaluate_subtraction(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_minus_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_minus_operator);
        struct Any result = None();
        for (size_t index = 0; index < split_elements->size; ++index) {
            struct Elements *group = &split_elements->data[index];
            struct Any group_result = with_queue(context, group, evaluate_multiplication);
            if (index == 0) {
                result = group_result;
            } else {
                result = Any_subtract(result, group_result);
            }
        }
        SplitElements_delete(split_elements);
        return result;
    } else {
        return evaluate_multiplication(context, queue);
    }
}

bool is_plus_operator(const struct Element *element) {
    return is_operator_with_text(element, "+");
}

struct Any evaluate_addition(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_plus_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_plus_operator);
        struct Any result = None();
        for (size_t index = 0; index < split_elements->size; ++index) {
            struct Elements *group = &split_elements->data[index];
            struct Any group_result = with_queue(context, group, evaluate_subtraction);
            if (index == 0) {
                result = group_result;
            } else {
                result = Any_add(result, group_result);
            }
        }
        SplitElements_delete(split_elements);
        return result;
    } else {
        return evaluate_subtraction(context, queue);
    }
}

bool is_comparison_operator(const struct Element *element) {
    return is_operator(element) &&
           (equal(element->token->text, "<") ||
            equal(element->token->text, ">") ||
            equal(element->token->text, "<=") ||
            equal(element->token->text, ">="));
}

struct Any evaluate_comparison(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_comparison_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_comparison_operator);
        if (split_elements->size != 2) {
            fail_at_position(split_elements->data[0].data[0].position, "Unsupported chained equality check");
        }
        const struct String *operator_name = split_elements->separators[0].token->text;
        struct Any lhs_result = with_queue(context, &split_elements->data[0], evaluate_addition);
        struct Any rhs_result = with_queue(context, &split_elements->data[1], evaluate_addition);
        SplitElements_delete(split_elements);
        if (equal(operator_name, "<")) {
            return Any_less_than(lhs_result, rhs_result);
        } else if (equal(operator_name, ">")) {
            return Any_greater_than(lhs_result, rhs_result);
        } else if (equal(operator_name, "<=>")) {
            return Any_less_than_or_equal(lhs_result, rhs_result);
        } else if (equal(operator_name, ">=")) {
            return Any_greater_than_or_equal(lhs_result, rhs_result);
        } else {
            fail("Logical error");
        }
    } else {
        return evaluate_addition(context, queue);
    }
}

bool is_equality_operator(const struct Element *element) {
    return is_operator(element) &&
           (equal(element->token->text, "==") || equal(element->token->text, "!="));
}

struct Any evaluate_equality(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_equality_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_equality_operator);
        if (split_elements->size != 2) {
            fail_at_position(split_elements->data[0].data[0].position, "Unsupported chained equality check");
        }
        bool equality_check = is_symbol_of_name(&split_elements->separators[0], "==");
        struct Any lhs_result = with_queue(context, &split_elements->data[0], evaluate_comparison);
        struct Any rhs_result = with_queue(context, &split_elements->data[1], evaluate_comparison);
        SplitElements_delete(split_elements);
        return equality_check ? Any_equal(lhs_result, rhs_result) : Any_unequal(lhs_result, rhs_result);
    } else {
        return evaluate_comparison(context, queue);
    }
}

struct Any evaluate_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Element *first_element = ElementQueue_peek(queue);
    if (is_bracket_element_of_type(first_element, Paren)) {
        const struct Elements *contained_expression = read_paren_block(queue);
        struct ElementQueue *contained_expression_queue = ElementQueue_create(contained_expression);
        struct Any result = evaluate_expression(context, contained_expression_queue);
        ElementQueue_delete(contained_expression_queue);
        return result;
    } else if (is_bracket_element_of_type(first_element, Square)) {
        // TODO list literal
        fail_at_position(first_element->position, "TODO array literal [1, \"2\", [3]]");
    } else if (is_bracket_element_of_type(first_element, Curly)) {
        // TODO list literal
        fail_at_position(first_element->position, "TODO map / set literal {a: 1, \"b\": 2, 3: 3} / {1, 2, 3}");
    } else if (ElementQueue_contains(queue, is_operator)) {
        return evaluate_equality(context, queue);
    } else {
        return evaluate_simple_expression(context, queue);
    }
}

void print(struct Variables *context, struct SplitElements *arguments) {
    for (size_t index = 0; index < arguments->size; ++index) {
        struct ElementQueue *argument_queue = ElementQueue_create(&arguments->data[index]);
        struct Any value = evaluate_expression(context, argument_queue);
        print_value(value);
    }
    fflush(stdout);
}

struct StatementResult {
    bool is_return;
    struct Any value;
};

struct StatementResult run_block(struct Variables *context, struct ElementQueue *queue);

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
    struct ElementQueue *body_queue = ElementQueue_create(function->body);
    struct Any function_result = run_block(locals, body_queue).value;
    ElementQueue_delete(body_queue);
    Variables_release(locals);
    struct FunctionCallResult result;
    result.type = Success;
    result.value = function_result;
    return result;
}

static struct FunctionCallResult
call(struct Variables *context, const struct String *name, struct Position position, struct ElementQueue *arguments) {
    if (equal(name, "print") || equal(name, "println")) {
        struct SplitElements *split_arguments = SplitElements_by_comma(arguments);
        print(context, split_arguments);
        if (equal(name, "println")) {
            putchar('\n');
        }
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

struct StatementResult run_with_queue(struct Variables *context, const struct Elements *elements,
                                      struct StatementResult (*function)(struct Variables *context,
                                                                         struct ElementQueue *queue)) {
    struct ElementQueue *queue = ElementQueue_create(elements);
    struct StatementResult result = function(context, queue);
    ElementQueue_delete(queue);
    return result;
}

struct StatementResult execute_statement(struct Variables *context, struct Elements *statement) {
    struct ElementQueue *queue = ElementQueue_create(statement);
    const struct Token *symbol_token = read_symbol(queue);
    const struct String *symbol = symbol_token->text;
    if (equal(symbol, "let")) {
        const struct String *name = read_symbol(queue)->text;
        read_operator(queue, "=");
        create_variable(context, name, evaluate_expression(context, queue));
    } else if (equal(symbol, "fn")) {
        fn(context, queue);
    } else if (equal(symbol, "return")) {
        return (struct StatementResult) {true, evaluate_expression(context, queue)};
    } else if (is_operator_with_text(ElementQueue_peek(queue), "=")) {
        ElementQueue_next(queue);
        if (!set_variable(context, symbol, evaluate_expression(context, queue))) {
            fail_at_position(symbol_token->position, "Undefined variable [%s]", symbol->value);
        }
    } else if (equal(symbol, "if")) {
        const struct Elements *condition = read_paren_block(queue);
        const struct Elements *positive_case = read_curly_block(queue);
        const struct Elements *negative_case = 0;
        if (is_symbol_of_name(ElementQueue_peek(queue), "else")) {
            read_symbol(queue);
            negative_case = read_curly_block(queue);
        }
        struct Any condition_result = with_queue(context, condition, evaluate_expression);
        if (Any_true(condition_result).boolean) {
            return run_with_queue(context, positive_case, run_block);
        } else if (negative_case) {
            return run_with_queue(context, negative_case, run_block);
        }
    } else {
        const struct Element *opening_bracket = ElementQueue_peek(queue);
        struct ElementQueue *arguments = ElementQueue_create(read_paren_block(queue));
        call(context, symbol, opening_bracket->position, arguments);
    }
    return (struct StatementResult) {false, None()};
}

struct StatementResult run_block(struct Variables *context, struct ElementQueue *queue) {
    struct StatementResult result = {false, None()};
    struct SplitElements *statements = SplitElements_by_line(queue);
    for (size_t index = 0; index < statements->size; ++index) {
        struct Elements *statement = &statements->data[index];
        size_t line = statement->data[0].position.line;
        if (DEBUG_ENABLED) {
            printf("line %zu: ", line);
            print_statement(statement);
        }
        struct StatementResult statement_result = execute_statement(context, statement);
        if (statement_result.is_return) {
            result = statement_result;
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
