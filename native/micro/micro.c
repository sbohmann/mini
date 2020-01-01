#include "micro.h"

#include <stdlib.h>
#include <string.h>

#include <core/errors.h>
#include <collections/hashmap.h>
#include <collections/struct.h>
#include <generated/element_queue.h>

#include <minic/expressions/expressions.h>
#include <generated/string_list.h>
#include <core/complex.h>
#include <core/allocate.h>
#include <minic/list.h>

#include "debug.h"
#include "print.h"
#include "variables.h"
#include "split_elements.h"
#include "function.h"

struct Variables *global_context;

static struct Variables *create_bindings(const struct Variables *context, const struct Elements *names) {
    struct ElementQueue *queue = ElementQueue_create(names);
    struct SplitElements *split_names = SplitElements_by_comma(queue);
    struct Variables *result = Variables_create(global_context);
    for (size_t index = 0; index < names->size; ++index) {
        struct Elements *part = split_names->data + index;
        if (part->size == 0) {
            fail("logical_error");
        } else if (part->size > 1) {
            fail_at_position(part->data[1].position, "Unexpected token");
        }
        const struct Element *name_element = part->data;
        if (name_element->type != TokenElement || name_element->token->type != Symbol) {
            fail_at_position(name_element->position, "Unexpected token");
        }
        const struct String *name = name_element->token->text;
        struct MapResult get_result = get_variable(context, name);
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

static struct FunctionCallResult call(struct Variables *context, struct Any function, struct Any name,
                                      struct Position position, struct ElementQueue *arguments);

struct Any with_queue(struct Variables *context, const struct Elements *elements,
                      struct Any (*function)(struct Variables *context, struct ElementQueue *queue)) {
    struct ElementQueue *queue = ElementQueue_create(elements);
    struct Any result = function(context, queue);
    ElementQueue_delete(queue);
    return result;
}

struct Any evaluate_expression(struct Variables *context, struct ElementQueue *queue);

struct Any read_struct_literal(struct Variables *context, const struct Elements *elements) {
    struct Struct *result = Struct_create();
    struct ElementQueue *queue = ElementQueue_create(elements);
    struct SplitElements *lines = SplitElements_by_line(queue);
    for (size_t index = 0; index < lines->size; ++index) {
        struct ElementQueue *line_queue = ElementQueue_create(lines->data + index);
        const struct String *name = read_symbol(line_queue)->text;
        read_operator_with_text(line_queue, ":");
        struct Any value = evaluate_expression(context, line_queue);
        Struct_put(result, name, value);
    }
    ElementQueue_delete(queue);
    return Complex((struct ComplexValue *) result);
}

struct Any evaluate_simple_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Token *first_token = read_token(queue);
    struct Any result;
    struct Any name = None();
    const struct Element *second_element = ElementQueue_peek(queue);
    if (first_token->type == Symbol) {
        if (equal(first_token->text, "fn")) {
            struct Function *function = read_function(context, queue);
            return Complex(&function->base);
        }
        if (equal(first_token->text, "struct") && is_bracket_element_of_type(second_element, Curly)) {
            ElementQueue_next(queue);
            result = read_struct_literal(context, second_element->bracket.elements);
        } else {
            struct MapResult variable = get_variable(context, first_token->text);
            if (variable.found) {
                result = variable.value;
            } else {
                fail_at_position(first_token->position, "Undefined variable [%s]", first_token->text->value);
            }
        }
        name = String(first_token->text);
    } else if (first_token->type == NumberLiteral || first_token->type == StringLiteral) {
        result = first_token->value;
    } else {
        fail_at_position(first_token->position, "Unexpected expression: [%s]", first_token->text->value);
    }
    while (true) {
        const struct Element *next_element = ElementQueue_peek(queue);
        if (next_element) {
            if (is_operator_with_text(next_element, ".")) {
                ElementQueue_next(queue);
                const struct String *element_name = read_symbol(queue)->text;
                if (result.type != ComplexType || result.complex_value->type != StructComplexType) {
                    fail_at_position(next_element->position, "Dot operator only valid on struct values");
                }
                struct Struct *container = (struct Struct *) result.complex_value;
                struct MapResult get_result = Struct_get(container, element_name);
                if (!get_result.found) {
                    fail_at_position(next_element->position, "Undefined symbol: %s", element_name->value);
                }
                result = get_result.value;
            } else if (is_bracket_element_of_type(next_element, Paren)) {
                const struct Elements *arguments = read_paren_block(queue);
                struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
                struct FunctionCallResult call_result =
                        call(context, result, name, next_element->position, arguments_queue);
                ElementQueue_delete(arguments_queue);
                if (call_result.type == Success) {
                    result = call_result.value;
                } else {
                    if (name.type == StringType) {
                        fail_at_position(next_element->position, "Call to %s failed.", name.string->value);
                    } else {
                        fail_at_position(next_element->position, "Call failed.");
                    }
                }
            } else if (is_bracket_element_of_type(next_element, Square)) {
                const struct Elements *arguments = read_square_block(queue);
                if (result.type != ComplexType || result.complex_value->type != ListComplexType) {
                    fail_at_position(next_element->position, "Index access not supported for type %s",
                                     Any_typename(result));
                }
                struct ElementQueue *index_queue = ElementQueue_create(arguments);
                struct Any index_value = evaluate_expression(context, index_queue);
                ElementQueue_delete(index_queue);
                if (index_value.type == IntegerType) {
                    struct List *list = (struct List *) result.complex_value;
                    result = List_get(list, index_value.integer);
                } else {
                    fail_at_position(next_element->position, "Illegal index value type %s",
                                     Any_typename(index_value));
                }
            } else {
                fail_at_position(next_element->position, "Unexpected token");
            }
        } else {
            break;
        }
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
            struct Elements *group = split_elements->data + index;
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
            struct Elements *group = split_elements->data + index;
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
            struct Elements *group = split_elements->data + index;
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
            struct Elements *group = split_elements->data + index;
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

bool compare_values(const struct String *operator_name, struct Any lhs, struct Any rhs) {
    if (equal(operator_name, "<")) {
        return Any_less_than(lhs, rhs);
    } else if (equal(operator_name, ">")) {
        return Any_greater_than(lhs, rhs);
    } else if (equal(operator_name, "<=>")) {
        return Any_less_than_or_equal(lhs, rhs);
    } else if (equal(operator_name, ">=")) {
        return Any_greater_than_or_equal(lhs, rhs);
    } else {
        fail("Logical error");
    }
}

struct Any evaluate_comparison(struct Variables *context, struct ElementQueue *queue) {
    if (ElementQueue_contains(queue, is_comparison_operator)) {
        struct SplitElements *split_elements = SplitElements_by_predicate(queue, is_comparison_operator);
        if (split_elements->size != 2) {
            fail_at_position(split_elements->data[0].data[0].position, "Unsupported chained equality check");
        }
        const struct String *operator_name = split_elements->separators[0].token->text;
        struct Any lhs_result = with_queue(context, split_elements->data, evaluate_addition);
        struct Any rhs_result = with_queue(context, split_elements->data + 1, evaluate_addition);
        SplitElements_delete(split_elements);
        return Boolean(compare_values(operator_name, lhs_result, rhs_result));
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
        bool equality_check = is_symbol_of_name(split_elements->separators, "==");
        struct Any lhs_result = with_queue(context, split_elements->data, evaluate_comparison);
        struct Any rhs_result = with_queue(context, split_elements->data + 1, evaluate_comparison);
        SplitElements_delete(split_elements);
        return Boolean(equality_check ?
                       Any_equal(lhs_result, rhs_result) :
                       Any_unequal(lhs_result, rhs_result));
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

struct Assignment {
    struct Variables *context;
    union {
        struct {
            const struct String *variable_name;
        };
        struct {
            struct Struct *struct_instance;
            const struct String *struct_key;
        };
        struct {
            struct List *list_instance;
            int64_t list_index;
        };
        const char *error_message;
    };
    void (*function)(struct Assignment, struct Any value);
};

void assign_to_variable(struct Assignment assignment, struct Any value) {
    set_variable(assignment.context, assignment.variable_name, value);
}

void assign_to_struct(struct Assignment assignment, struct Any value) {
    Struct_put(assignment.struct_instance, assignment.struct_key, value);
}

void assign_to_list(struct Assignment assignment, struct Any value) {
    List_set(assignment.list_instance, assignment.list_index, value);
}

struct Assignment evaluate_lhs_expression(struct Variables *context, struct ElementQueue *queue) {
    struct Assignment assignment;
    memset(&assignment, 0, sizeof(struct Assignment));
    const struct Token *first_token = read_token(queue);
    struct Any result = None();
    struct Any name = None();
    const struct Element *second_element = ElementQueue_peek(queue);
    if (first_token->type == Symbol) {
        if (equal(first_token->text, "fn")) {
            assignment.error_message = "Assignment to function";
            return assignment;
        }
        if (equal(first_token->text, "struct") && is_bracket_element_of_type(second_element, Curly)) {
            assignment.error_message = "Assignment to struct";
            return assignment;
        } else {
            struct MapResult variable = get_variable(context, first_token->text);
            if (variable.found) {
                if (variable.found) {
                    result = variable.value;
                    if (!ElementQueue_peek(queue)) {
                        assignment.variable_name = first_token->text;
                        assignment.function = assign_to_variable;
                        return assignment;
                    }
                } else {
                    fail_at_position(first_token->position, "Undefined variable [%s]", first_token->text->value);
                }
            } else {
                fail_at_position(first_token->position, "Undefined variable [%s]", first_token->text->value);
            }
        }
        name = String(first_token->text);
    } else if (first_token->type == NumberLiteral || first_token->type == StringLiteral) {
        assignment.error_message = "Assignment to literal value";
        return assignment;
    } else {
        fail_at_position(first_token->position, "Unexpected expression: [%s]", first_token->text->value);
    }
    while (true) {
        const struct Element *next_element = ElementQueue_peek(queue);
        if (next_element) {
            if (is_operator_with_text(next_element, ".")) {
                ElementQueue_next(queue);
                const struct String *element_name = read_symbol(queue)->text;
                if (result.type != ComplexType || result.complex_value->type != StructComplexType) {
                    fail_at_position(next_element->position, "Dot operator only valid on struct values");
                }
                struct Struct *container = (struct Struct *) result.complex_value;
                if (ElementQueue_peek(queue)) {
                    struct MapResult get_result = Struct_get(container, element_name);
                    if (!get_result.found) {
                        fail_at_position(next_element->position, "Undefined symbol: %s", element_name->value);
                    }
                    result = get_result.value;
                } else {
                    assignment.struct_instance = container;
                    assignment.struct_key = element_name;
                    assignment.function = assign_to_struct;
                    return assignment;
                }
            } else if (is_bracket_element_of_type(next_element, Paren)) {
                const struct Elements *arguments = read_paren_block(queue);
                if (ElementQueue_peek(queue)) {
                    struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
                    struct FunctionCallResult call_result =
                            call(context, result, name, next_element->position, arguments_queue);
                    ElementQueue_delete(arguments_queue);
                    if (call_result.type == Success) {
                        result = call_result.value;
                    } else {
                        if (name.type == StringType) {
                            fail_at_position(next_element->position, "Call to %s failed.", name.string->value);
                        } else {
                            fail_at_position(next_element->position, "Call failed.");
                        }
                    }
                } else {
                    assignment.error_message = "Assignment to function call result";
                    return assignment;
                }
            } else if (is_bracket_element_of_type(next_element, Square)) {
                const struct Elements *arguments = read_square_block(queue);
                if (result.type != ComplexType || result.complex_value->type != ListComplexType) {
                    fail_at_position(next_element->position, "Index access not supported for type %s",
                                     Any_typename(result));
                }
                struct ElementQueue *index_queue = ElementQueue_create(arguments);
                struct Any index_value = evaluate_expression(context, index_queue);
                ElementQueue_delete(index_queue);
                if (index_value.type != IntegerType) {
                    fail_at_position(next_element->position, "Illegal index value type %s",
                                     Any_typename(index_value));
                }
                struct List *list = (struct List *) result.complex_value;
                if (ElementQueue_peek(queue)) {
                    result = List_get(list, index_value.integer);
                } else {
                    assignment.list_instance = list;
                    assignment.list_index = index_value.integer;
                    assignment.function = assign_to_list;
                    return assignment;
                }
            } else {
                fail_at_position(next_element->position, "Unexpected token");
            }
        } else {
            fail("Logical error");
        }
    }
}

static void print_raw(const struct List *arguments) {
    for (size_t index = 0; index < arguments->size; ++index) {
        print_value(List_get(arguments, index));
    }
}

struct Any print(const struct List *arguments) {
    print_raw(arguments);
    fflush(stdout);
    return None();
}

struct Any println(const struct List *arguments) {
    print_raw(arguments);
    putchar('\n');
    fflush(stdout);
    return None();
}

struct Any list(const struct List *value) {
    struct List *result = List_copy(value);
    return Complex(&result->base);
}

struct StatementResult {
    bool is_return;
    struct Any value;
};

struct StatementResult run_block(struct Variables *context, struct ElementQueue *queue);

static struct FunctionCallResult call_function(struct Variables *context, struct Function *function,
                                               struct List *arguments) {
    if (arguments->size != StringList_size(function->parameter_names)) {
        struct FunctionCallResult result;
        result.type = ArgumentNumberMismatch;
        result.arguments_passed = arguments->size;
        result.arguments_expected = StringList_size(function->parameter_names);
        return result;
    }
    struct Variables *locals = Variables_create(function->bindings ? function->bindings : global_context);
    struct StringListElement *name_iterator = StringList_begin(function->parameter_names);
    size_t index = 0;
    while (name_iterator) {
        const struct String *name = StringListIterator_get(name_iterator);
        if (index >= arguments->size) {
            fail("Logical error");
        }
        struct Any value = List_get(arguments, index);
        create_variable(locals, name, value);
        name_iterator = StringListIterator_next(name_iterator);
        ++index;
    }
    if (index != arguments->size) {
        fail("Logical error");
    }
    struct ElementQueue *body_queue = ElementQueue_create(function->body);
    struct Any function_result = run_block(locals, body_queue).value;
    ElementQueue_delete(body_queue);
    Variables_release(locals);
    struct FunctionCallResult result;
    result.type = Success;
    result.value = function_result;
    return result;
}

struct List *create_list(struct Variables *context, struct SplitElements *split_arguments) {
    struct List *result = List_create();
    for (size_t index = 0; index < split_arguments->size; ++index) {
        struct Elements *group = split_arguments->data + index;
        struct ElementQueue *group_queue = ElementQueue_create(group);
        struct Any value = evaluate_expression(context, group_queue);
        List_add(result, value);
        Any_release(value);
        ElementQueue_delete(group_queue);
    }
    return result;
}

static struct FunctionCallResult call(struct Variables *context, struct Any function, struct Any name,
                                      struct Position position, struct ElementQueue *arguments) {
    struct SplitElements *split_arguments = SplitElements_by_comma(arguments);
    struct List *argument_list = create_list(context, split_arguments);
    SplitElements_delete(split_arguments);
    struct FunctionCallResult result = {Error};
    if (function.type == FunctionType) {
        result.type = Success;
        result.value = function.function(argument_list);
    } else if (function.type == ComplexType) {
        if (function.complex_value->type == FunctionComplexType) {
            struct Function *complex_function = (struct Function *) function.complex_value;
            result = call_function(context, complex_function, argument_list);
            if (result.type == ArgumentNumberMismatch) {
                if (name.type == StringType) {
                    printf("Argument number mismatch in call to function %s - %zu arguments passed, %zu expected.\n",
                           name.string->value, result.arguments_passed, result.arguments_expected);
                } else {
                    printf("Argument number mismatch in call to function - %zu arguments passed, %zu expected.\n",
                           result.arguments_passed, result.arguments_expected);
                }
            }
        } else {
            printf("Error: failed to call non-function complex value\n");
        }
    } else {
        printf("Error: failed to call non-function value of type %s\n", Any_typename(function));
    }
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

bool is_assignment_operator(const struct Element *element) {
    return is_operator_with_text(element, "=");
}

struct SplitAssignment {
    struct Element *assignment_operator;
    struct Elements *lhs;
    struct Elements *rhs;
    struct SplitElements *split_elements;
};

struct SplitAssignment split_assignment(struct Elements *statement) {
    struct ElementQueue *queue = ElementQueue_create(statement);
    struct SplitElements *split_elements = SplitElements_by_operator(queue, "=");
    if (split_elements->size < 2) {
        if (statement->size < 1) {
            fail("logical error");
        }
        fail_at_position(statement->data[statement->size - 1].position, "Unexpected end of line");
    } else if (split_elements->size > 2) {
        fail_at_position(split_elements->separators[1].position, "Multiple assignments are not supported");
    }
    struct SplitAssignment result = (struct SplitAssignment) {
        split_elements->separators, split_elements->data, split_elements->data + 1, split_elements};
    return result;
}

struct StatementResult execute_statement(struct Variables *context, struct Elements *statement) {
    struct ElementQueue *queue = ElementQueue_create(statement);
    const struct Token *symbol_token = read_symbol(queue);
    const struct String *symbol = symbol_token->text;
    if (equal(symbol, "let")) {
        const struct String *name = read_symbol(queue)->text;
        read_operator_with_text(queue, "=");
        create_variable(context, name, evaluate_expression(context, queue));
    } else if (equal(symbol, "fn")) {
        fn(context, queue);
    } else if (equal(symbol, "return")) {
        // TODO implement tail call
        return (struct StatementResult) {true, evaluate_expression(context, queue)};
    } else if (is_operator_with_text(ElementQueue_peek(queue), "=")) {
        ElementQueue_next(queue);
        if (!set_variable(context, symbol, evaluate_expression(context, queue))) {
            fail_at_position(symbol_token->position, "Undefined variable [%s]", symbol->value);
        }
    } else if (ElementQueue_contains(queue, is_assignment_operator)) {
        struct SplitAssignment sides = split_assignment(statement);
        struct ElementQueue *lhs_queue = ElementQueue_create(sides.lhs);
        struct Assignment assignment = evaluate_lhs_expression(context, lhs_queue);
        ElementQueue_delete(lhs_queue);
        if (!assignment.function) {
            if (assignment.error_message) {
                fail_at_position(sides.assignment_operator->position, "%s", assignment.error_message);
            } else {
                fail("Logical error");
            }
        }
        struct ElementQueue *rhs_queue = ElementQueue_create(sides.rhs);
        struct Any value = evaluate_expression(context, rhs_queue);
        ElementQueue_delete(rhs_queue);
        SplitElements_delete(sides.split_elements);
        assignment.function(assignment, value);
    } else if (equal(symbol, "if")) {
        const struct Elements *condition = read_paren_block(queue);
        const struct Elements *positive_case = read_curly_block(queue);
        const struct Elements *negative_case = 0;
        if (is_symbol_of_name(ElementQueue_peek(queue), "else")) {
            read_symbol(queue);
            negative_case = read_curly_block(queue);
        }
        struct Any condition_result = with_queue(context, condition, evaluate_expression);
        if (Any_true(condition_result)) {
            return run_with_queue(context, positive_case, run_block);
        } else if (negative_case) {
            return run_with_queue(context, negative_case, run_block);
        }
    } else if (equal(symbol, "while")) {
        const struct Elements *condition = read_paren_block(queue);
        const struct Elements *positive_case = read_curly_block(queue);
        while (true) {
            struct Any condition_result = with_queue(context, condition, evaluate_expression);
            if (Any_true(condition_result)) {
                struct StatementResult while_block_result = run_with_queue(context, positive_case, run_block);
                if (while_block_result.is_return) {
                    return while_block_result;
                }
            } else {
                break;
            }
        }
    } else {
        ElementQueue_reset(queue);
        evaluate_expression(context, queue);
    }
    return (struct StatementResult) {false, None()};
}

struct StatementResult run_block(struct Variables *context, struct ElementQueue *queue) {
    struct StatementResult result = {false, None()};
    struct SplitElements *statements = SplitElements_by_line(queue);
    for (size_t index = 0; index < statements->size; ++index) {
        struct Elements *statement = statements->data + index;
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
    create_variable(globals, String_from_literal("print"), Function(print));
    create_variable(globals, String_from_literal("println"), Function(println));
    create_variable(globals, String_from_literal("list"), Function(list));
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
