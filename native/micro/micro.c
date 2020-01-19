#include "micro.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <core/errors.h>
#include <core/complex.h>
#include <core/allocate.h>
#include <core/numbers.h>
#include <generated/string_list.h>
#include <collections/hashmap.h>
#include <collections/hashset.h>
#include <collections/struct.h>
#include <minic/elements/element_queue.h>
#include <minic/expressions/expressions.h>
#include <minic/list.h>

#include "debug.h"
#include "print.h"
#include "variables.h"
#include "split_elements.h"
#include "function.h"
#include "fs.h"
#include "micro/methods/string_methods.h"

struct Variables *global_context;

struct Any handle_error(struct Position position, struct Any value) {
    if (value.type == ErrorType) {
        fail_at_position(position, "%s", value.string->value);
    } else {
        return value;
    }
}

static struct Variables *create_bindings(const struct Variables *context, const struct Elements *names) {
    struct ElementQueue *queue = ElementQueue_create(names);
    struct SplitElements *split_names = SplitElements_by_comma(queue);
    struct Variables *result = Variables_create(global_context);
    for (size_t index = 0; index < names->size; ++index) {
        struct Elements *part = split_names->data + index;
        if (part->size == 0) {
            fail_with_message("logical_error");
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

struct Function *read_function(const struct Variables *context, struct ElementQueue *queue, const struct String *name) {
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
    return Function_create(bindings, parameter_names, body, name);
}

static void fn(struct Variables *context, struct ElementQueue *queue) {
    const struct String *name = read_symbol(queue)->text;
    struct Function *function = read_function(context, queue, name);
    create_variable(context, name, Complex(&function->base));
}

enum FunctionCallResultType {
    SuccessFunctionResult,
    ErrorFunctionResult,
    ArgumentNumberMismatchFunctionResult
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

static struct FunctionCallResult
parse_arguments_and_call(struct Variables *context, struct Any function, struct Any name,
                         struct Position position, struct ElementQueue *arguments);

static struct FunctionCallResult call(struct Variables *context, struct Any function, struct Any name,
                                      struct Position position, struct List *arguments);

static struct Any call_or_fail(struct Variables *context, struct Any function, struct Any name,
                               struct Position position, struct List *arguments);

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

struct List *evaluate_arguments(struct Variables *context, struct SplitElements *split_arguments) {
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

struct List *read_arguments(struct Variables *context, struct ElementQueue *queue) {
    struct SplitElements *split_arguments = SplitElements_by_comma(queue);
    struct List *argument_list = evaluate_arguments(context, split_arguments);
    SplitElements_delete(split_arguments);
    return argument_list;
}

bool has_complex_type(struct Any value, enum ComplexType type) {
    return value.type == ComplexType &&
           value.complex_value->type == type;
}

static void check_arguments(const char *name, const struct List *arguments, size_t number, ...) {
    va_list argp;
    va_start(argp, number);
    if (arguments->size != number) {
        fail_with_message("Argument number mismatch in call to %s - %zu arguments passed, %zu expected",
                          name, arguments->size, number);
    }
    for (size_t index = 0; index < number; ++index) {
        struct Any argument = List_get(arguments, index);
        enum AnyType type = va_arg(argp, enum AnyType);
        if (type == ComplexType) {
            enum ComplexType complex_type = va_arg(argp, enum ComplexType);
            if (argument.type != ComplexType || argument.complex_value->type != complex_type) {
                fail_with_message("Argument %zu in call to %s has illegal type %s - expecting %s", index + 1, name,
                                  Any_typename(argument), ComplexType_to_string(complex_type));
            }
        } else {
            if (argument.type != type) {
                fail_with_message("Argument %zu in call to %s has illegal type %s - expecting %s", index + 1, name,
                                  Any_typename(argument), AnyType_to_string(type));
            }
        }
    }
    va_end(argp);
}

static void check_arguments_with_position(struct Position position, const char *name,
                                          const struct List *arguments, size_t number, ...) {
    va_list argp;
    va_start(argp, number);
    if (arguments->size != number) {
        fail_at_position(
                position,
                "Argument number mismatch in call to %s - %zu arguments passed, %zu expected",
                name, arguments->size, number);
    }
    for (size_t index = 0; index < number; ++index) {
        struct Any argument = List_get(arguments, index);
        enum AnyType type = va_arg(argp, enum AnyType);
        if (type == ComplexType) {
            enum ComplexType complex_type = va_arg(argp, enum ComplexType);
            if (argument.type != ComplexType || argument.complex_value->type != complex_type) {
                fail_at_position(
                        position,
                        "Argument %zu in call to %s has illegal type %s - expecting %s", index + 1, name,
                        Any_typename(argument), ComplexType_to_string(complex_type));
            }
        } else {
            if (argument.type != type) {
                fail_at_position(
                        position,
                        "Argument %zu in call to %s has illegal type %s - expecting %s", index + 1, name,
                        Any_typename(argument), AnyType_to_string(type));
            }
        }
    }
    va_end(argp);
}

struct Any call_method(struct Variables *context, struct Any instance, const struct String *name,
                       struct Position position, struct ElementQueue *queue) {
    struct Any result = instance;
    struct ElementQueue *arguments_queue = ElementQueue_create(read_paren_block(queue));
    struct List *arguments = read_arguments(context, arguments_queue);
    if (has_complex_type(instance, SetComplexType) && equal(name, "contains")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to HashSet.contains - "
                             "%zu arguments passed, 1 expected.\n",
                             arguments->size);
        }
        result = Boolean(HashSet_contains((struct HashSet *) instance.complex_value,
                                          List_get(arguments, 0)));
    } else if (has_complex_type(instance, SetComplexType) && equal(name, "add")) {
        for (size_t index = 0; index < arguments->size; ++index) {
            HashSet_add((struct HashSet *) instance.complex_value,
                        List_get(arguments, index));
        }
    } else if (has_complex_type(instance, MapComplexType) && equal(name, "put")) {
        if (arguments->size % 2 != 0) {
            fail_at_position(position,
                             "Odd number of arguments passed to HashMap.put: %zu", arguments->size);
        }
        for (size_t index = 0; index < arguments->size; index += 2) {
            HashMap_put((struct HashMap *) instance.complex_value,
                        List_get(arguments, index), List_get(arguments, index + 1));
        }
    } else if (has_complex_type(instance, MapComplexType) && equal(name, "get")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to HashSet.contains - %zu arguments passed, 1 expected",
                             arguments->size);
        }
        struct MapResult map_result = HashMap_get((struct HashMap *) instance.complex_value,
                                                  List_get(arguments, 0));
        if (map_result.found) {
            result = map_result.value;
        } else {
            result = None();
        }
    } else if (has_complex_type(instance, MapComplexType) && equal(name, "at")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to HashSet.contains - %zu arguments passed, 1 expected",
                             arguments->size);
        }
        struct MapResult map_result = HashMap_get((struct HashMap *) instance.complex_value,
                                                  List_get(arguments, 0));
        struct Struct *map_result_struct = Struct_create();
        Struct_put(map_result_struct, String_from_literal("found"), Boolean(map_result.found));
        Struct_put(map_result_struct, String_from_literal("value"), map_result.value);
        result = Complex(&map_result_struct->base);
    } else if (has_complex_type(instance, ListComplexType) && equal(name, "map")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to List.map - %zu arguments passed, 1 expected",
                             arguments->size);
        }
        struct Any function = List_get(arguments, 0);
        struct List *input = (struct List *) instance.complex_value;
        struct List *map_result = List_create();
        struct Any function_name = None();
        if (function.type == FunctionPointerType) {
            function_name = String(function.function_name);
        } else if (function.type == ComplexType && function.complex_value->type == FunctionComplexType) {
            const struct String *raw_name = ((struct Function *) function.complex_value)->name;
            if (raw_name != 0) {
                function_name = String(raw_name);
            }
        }
        for (size_t index = 0; index < input->size; ++index) {
            struct List *function_arguments = List_create();
            List_add(function_arguments, List_get(input, index));
            List_add(map_result, call_or_fail(context, function, function_name, position, function_arguments));
            release(&function_arguments->base);
        }
        return Complex(&map_result->base);
    } else if (has_complex_type(instance, ListComplexType) && equal(name, "filter")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to List.map - %zu arguments passed, 1 expected",
                             arguments->size);
        }
        struct Any function = List_get(arguments, 0);
        struct List *input = (struct List *) instance.complex_value;
        struct List *filter_result = List_create();
        struct Any function_name = None();
        if (function.type == FunctionPointerType) {
            function_name = String(function.function_name);
        } else if (function.type == ComplexType && function.complex_value->type == FunctionComplexType) {
            const struct String *raw_name = ((struct Function *) function.complex_value)->name;
            if (raw_name != 0) {
                function_name = String(raw_name);
            }
        }
        for (size_t index = 0; index < input->size; ++index) {
            struct List *function_arguments = List_create();
            struct Any value = List_get(input, index);
            List_add(function_arguments, value);
            struct Any function_result = call_or_fail(context, function, function_name, position, function_arguments);
            if (function_result.type != BooleanType) {
                fail_with_message("Function passed to filter returned a non-boolean value of type %s",
                                  Any_typename(function_result));
            }
            if (function_result.boolean) {
                List_add(filter_result, value);
            }
            release(&function_arguments->base);
        }
        return Complex(&filter_result->base);
    } else if (has_complex_type(instance, ListComplexType) && equal(name, "foreach")) {
        if (arguments->size != 1) {
            fail_at_position(position,
                             "Argument number mismatch in call to List.map - %zu arguments passed, 1 expected",
                             arguments->size);
        }
        struct Any function = List_get(arguments, 0);
        struct List *input = (struct List *) instance.complex_value;
        struct Any function_name = None();
        if (function.type == FunctionPointerType) {
            function_name = String(function.function_name);
        } else if (function.type == ComplexType && function.complex_value->type == FunctionComplexType) {
            const struct String *raw_name = ((struct Function *) function.complex_value)->name;
            if (raw_name != 0) {
                function_name = String(raw_name);
            }
        }
        for (size_t index = 0; index < input->size; ++index) {
            struct List *function_arguments = List_create();
            struct Any value = List_get(input, index);
            List_add(function_arguments, value);
            call_or_fail(context, function, function_name, position, function_arguments);
            release(&function_arguments->base);
        }
        return None();
    } else if (instance.type == StringType && equal(name, "split")) {
        check_arguments_with_position(position, "String.split", arguments, 1, StringType, StringType);
        const struct String *separator = List_get(arguments, 0).string;
        return String_split(instance.string, separator);
    } else if (instance.type == StringType && equal(name, "trim")) {
        check_arguments_with_position(position, "String.split", arguments, 0);
        return String_trim(instance.string);
    } else {
        fail_at_position(position, "Call to undefined method %s.%s", Any_typename(instance), name->value);
    }
    release(&arguments->base);
    if (Any_raw_equal(result, instance)) {
        Any_retain(result);
    }
    return result;
}

struct Any read_property(struct Variables *context, struct Any instance, const struct String *name,
                         struct Position position) {
    struct Any result = instance;
    if (has_complex_type(instance, ListComplexType) && equal(name, "size")) {
        result = Integer(((struct List *) instance.complex_value)->size);
    } else if (has_complex_type(instance, SetComplexType) && equal(name, "size")) {
        result = Integer(((struct HashSet *) instance.complex_value)->size);
    } else if (has_complex_type(instance, MapComplexType) && equal(name, "size")) {
        result = Integer(((struct HashMap *) instance.complex_value)->size);
    } else if (has_complex_type(instance, StringComplexType) && equal(name, "length")) {
        // TODO
        result = Integer(0);
    } else if (instance.type == StringType && equal(name, "length")) {
        result = Integer(((struct String *) instance.complex_value)->length);
    } else {
        fail_at_position(position, "Undefined property %s.%s", Any_typename(instance), name->value);
    }
    if (Any_raw_equal(result, instance)) {
        Any_retain(result);
    }
    return result;
}

struct Any evaluate_simple_expression(struct Variables *context, struct ElementQueue *queue) {
    const struct Token *first_token = read_token(queue);
    struct Any result;
    struct Any name = None();
    const struct Element *second_element = ElementQueue_peek(queue);
    if (first_token->type == Symbol) {
        if (equal(first_token->text, "fn")) {
            struct Function *function = read_function(context, queue, 0);
            return Complex(&function->base);
        }
        if (equal(first_token->text, "struct") && is_bracket_element_of_type(second_element, Curly)) {
            ElementQueue_next(queue);
            result = read_struct_literal(context, second_element->bracket->elements);
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
    } else if (first_token->type == Operator && equal(first_token->text, "!")) {
        result = Not(evaluate_simple_expression(context, queue));
    } else {
        fail_at_position(first_token->position, "Unexpected expression: [%s]", first_token->text->value);
    }
    while (true) {
        const struct Element *next_element = ElementQueue_peek(queue);
        if (next_element) {
            if (is_operator_with_text(next_element, ".")) {
                ElementQueue_next(queue);
                const struct String *element_name = read_symbol(queue)->text;
                if (result.type == ComplexType && result.complex_value->type == StructComplexType) {
                    struct Struct *container = (struct Struct *) result.complex_value;
                    struct MapResult get_result = Struct_get(container, element_name);
                    if (!get_result.found) {
                        fail_at_position(next_element->position, "Undefined symbol: %s", element_name->value);
                    }
                    result = get_result.value;
                } else {
                    if (is_bracket_element_of_type(ElementQueue_peek(queue), Paren)) {
                        result = call_method(context, result, element_name, next_element->position, queue);
                    } else {
                        result = read_property(context, result, element_name, next_element->position);
                    }
                }
            } else if (is_bracket_element_of_type(next_element, Paren)) {
                const struct Elements *arguments = read_paren_block(queue);
                struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
                struct FunctionCallResult call_result =
                        parse_arguments_and_call(context, result, name, next_element->position, arguments_queue);
                ElementQueue_delete(arguments_queue);
                if (call_result.type == SuccessFunctionResult) {
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
                result = handle_error(group->data[index].position, Any_divide(result, group_result));
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
                result = handle_error(group->data[index].position, Any_multiply(result, group_result));
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
                result = handle_error(group->data[index].position, Any_subtract(result, group_result));
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
                result = handle_error(group->data[index].position, Any_add(result, group_result));
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

struct Any compare_values(const struct String *operator_name, struct Any lhs, struct Any rhs) {
    if (equal(operator_name, "<")) {
        return Any_less_than(lhs, rhs);
    } else if (equal(operator_name, ">")) {
        return Any_greater_than(lhs, rhs);
    } else if (equal(operator_name, "<=")) {
        return Any_less_than_or_equal(lhs, rhs);
    } else if (equal(operator_name, ">=")) {
        return Any_greater_than_or_equal(lhs, rhs);
    } else {
        fail_with_message("Logical error");
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
        struct Any result = handle_error(split_elements->data[0].data[0].position, compare_values(operator_name, lhs_result, rhs_result));
        SplitElements_delete(split_elements);
        return result;
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
        return Boolean(equality_check == Any_raw_equal(lhs_result, rhs_result));
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
                name = String(element_name);
            } else if (is_bracket_element_of_type(next_element, Paren)) {
                const struct Elements *arguments = read_paren_block(queue);
                if (ElementQueue_peek(queue)) {
                    struct ElementQueue *arguments_queue = ElementQueue_create(arguments);
                    struct FunctionCallResult call_result =
                            parse_arguments_and_call(context, result, name, next_element->position, arguments_queue);
                    ElementQueue_delete(arguments_queue);
                    if (call_result.type == SuccessFunctionResult) {
                        result = call_result.value;
                    } else {
                        if (name.type == StringType) {
                            fail_at_position(next_element->position, "Call to %s failed.", name.string->value);
                        } else {
                            fail_at_position(next_element->position, "Call failed.");
                        }
                    }
                    name = None();
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
                name = None();
            } else {
                fail_at_position(next_element->position, "Unexpected token");
            }
        } else {
            fail_with_message("Logical error");
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

struct Any list(const struct List *elements) {
    struct List *result = List_copy(elements);
    return Complex(&result->base);
}

struct Any hashset(const struct List *elements) {
    struct HashSet *result = HashSet_create(elements);
    for (size_t index = 0; index < elements->size; ++index) {
        HashSet_add(result, List_get(elements, index));
    }
    return Complex(&result->base);
}

struct Any hashmap(const struct List *pairs) {
    struct HashMap *result = HashMap_create();
    if (pairs->size % 2 != 0) {
        fail_with_message("Odd number of arguments passed to HashMap constructor: %d", pairs->size);
    }
    for (size_t index = 0; index < pairs->size; index += 2) {
        HashMap_put(result, List_get(pairs, index), List_get(pairs, index + 1));
    }
    return Complex(&result->base);
}

struct Any parse_integer(const struct List *arguments) {
    check_arguments("parse_integer", arguments, 1, StringType);
    const struct String *input = List_get(arguments, 0).string;
    struct IntegerParsingResult result = parse_int64(input->value, input->length, 10);
    if (result.success) {
        return Integer(result.signed_result);
    } else {
        return Error("%s", result.error_message);
    }
}

struct Any micro_fail(const struct List *arguments) {
    println(arguments);
    fail();
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
        result.type = ArgumentNumberMismatchFunctionResult;
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
            fail_with_message("Logical error");
        }
        struct Any value = List_get(arguments, index);
        create_variable(locals, name, value);
        name_iterator = StringListIterator_next(name_iterator);
        ++index;
    }
    if (index != arguments->size) {
        fail_with_message("Logical error");
    }
    struct ElementQueue *body_queue = ElementQueue_create(function->body);
    struct Any function_result = run_block(locals, body_queue).value;
    ElementQueue_delete(body_queue);
    Variables_release(locals);
    struct FunctionCallResult result;
    result.type = SuccessFunctionResult;
    result.value = function_result;
    return result;
}

static struct FunctionCallResult
parse_arguments_and_call(struct Variables *context, struct Any function, struct Any name,
                         struct Position position, struct ElementQueue *arguments) {
    struct List *argument_list = read_arguments(context, arguments);
    struct FunctionCallResult result = call(context, function, name, position, argument_list);
    free(argument_list);
    return result;
}

static struct FunctionCallResult call(struct Variables *context, struct Any function, struct Any name,
                                      struct Position position, struct List *arguments) {
    struct FunctionCallResult result = {ErrorFunctionResult};
    if (function.type == FunctionPointerType) {
        result.type = SuccessFunctionResult;
        result.value = function.function(arguments);
    } else if (function.type == ComplexType) {
        if (function.complex_value->type == FunctionComplexType) {
            struct Function *complex_function = (struct Function *) function.complex_value;
            result = call_function(context, complex_function, arguments);
            if (result.type == ArgumentNumberMismatchFunctionResult) {
                if (name.type == StringType) {
                    fail_at_position(position,
                                     "Argument number mismatch in call to function %s - %zu arguments passed, %zu expected.",
                                     name.string->value, result.arguments_passed, result.arguments_expected);
                } else {
                    fail_at_position(position,
                                     "Argument number mismatch in call to function - %zu arguments passed, %zu expected.",
                                     result.arguments_passed, result.arguments_expected);
                }
            }
        } else {
            fail_at_position(position, "Error: failed to call non-function complex value of type %s\n",
                             Any_typename(function));
        }
    } else {
        fail_at_position(position, "Error: failed to call non-function value of type %s\n", Any_typename(function));
    }
    return result;
}

static struct Any call_or_fail(struct Variables *context, struct Any function, struct Any name,
                               struct Position position, struct List *arguments) {
    struct FunctionCallResult call_result = call(context, function, name, position, arguments);
    if (call_result.type == SuccessFunctionResult) {
        return call_result.value;
    } else {
        if (name.type == StringType) {
            fail_at_position(position, "Call to function %s failed", name.string->value);
        } else {
            fail_at_position(position, "Call to function failed");
        }
    }
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
            printf("%s", bracket_type_name(element.bracket->type));
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
            fail_with_message("logical error");
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
                fail_with_message("Logical error");
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
        if (Any_raw_true(condition_result)) {
            return run_with_queue(context, positive_case, run_block);
        } else if (negative_case) {
            return run_with_queue(context, negative_case, run_block);
        }
    } else if (equal(symbol, "while")) {
        const struct Elements *condition = read_paren_block(queue);
        const struct Elements *positive_case = read_curly_block(queue);
        while (true) {
            struct Any condition_result = with_queue(context, condition, evaluate_expression);
            if (Any_raw_true(condition_result)) {
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

void create_builtin_function(struct Variables *context, const char *name, struct Any (*function)(const struct List *)) {
    create_constant(context, String_from_literal(name), Function(function, name));
}

void micro_run(struct ParsedModule *module) {
    struct Variables *globals = Variables_create(0);
    global_context = globals;
    create_builtin_function(globals, "print", print);
    create_builtin_function(globals, "println", println);
    create_builtin_function(globals, "List", list);
    create_builtin_function(globals, "HashSet", hashset);
    create_builtin_function(globals, "HashMap", hashmap);
    create_builtin_function(globals, "parse_integer", parse_integer);
    create_builtin_function(globals, "fail", parse_integer);
    create_constant(globals, String_from_literal("true"), True());
    create_constant(globals, String_from_literal("false"), False());
    create_constant(globals, String_from_literal("None"), None());
    // TODO move to a module
    create_builtin_function(globals, "read_text_file", micro_read_text_file);
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    run_block(globals, queue);
    ElementQueue_delete(queue);
    global_context = 0;
    Variables_release(globals);
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fail_with_message("Expecting single argument <path to source file>");
    }
    struct ParsedModule *module = read_file(argv[1]);
    micro_run(module);
}
