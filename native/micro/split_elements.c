#include "split_elements.h"

#include <stdlib.h>

#include <generated/element_list.h>
#include <minic/elements/element_queue.h>
#include <minic/expressions/expressions.h>
#include <generated/elements_list.h>
#include <core/allocate.h>
#include <core/errors.h>

static struct SplitElements *
SplitElements_from_lists(const struct ElementsList *groups, const struct ElementList *separators) {
    struct SplitElements *result = allocate(sizeof(struct SplitElements));
    result->size = ElementsList_size(groups);
    result->data = ElementsList_to_array(groups);
    if (separators && result->size != 0) {
        size_t number_of_separators = ElementList_size(separators);
        if (number_of_separators == result->size) {
            fail_at_position(ElementList_to_array(separators)[number_of_separators - 1].position, "Dangling separator");
        } else if (number_of_separators != result->size - 1) {
            fail_with_message("Logical error - %d groups %d separators (expected %d)",
                              result->size, number_of_separators, result->size - 1);
        }
        result->separators = ElementList_to_array(separators);
    }
    return result;
}

void SplitElements_delete(struct SplitElements *instance) {
    deallocate(instance->data);
    deallocate(instance->separators);
    deallocate(instance);
}

bool is_comma(const struct Element *element) {
    return element->type == TokenElement &&
           element->token->type == OperatorToken &&
           equal(element->token->text, ",");
}

struct SplitElements *SplitElements_by_comma(struct ElementQueue *queue) {
    struct ElementsList *raw_result = ElementsList_create();
    struct ElementList *separators = ElementList_create();
    struct ElementList *group = ElementList_create();
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (is_comma(element)) {
            if (ElementList_size(group) == 0) {
                fail_at_position(element->position, "Unexpected token");
            }
            ElementsList_append(raw_result, Elements_from_list(group));
            ElementList_delete(group);
            group = ElementList_create();
            ElementList_append(separators, element);
        } else {
            ElementList_append(group, element);
        }
    }
    if (ElementList_size(group) > 0) {
        ElementsList_append(raw_result, Elements_from_list(group));
    }
    ElementList_delete(group);
    struct SplitElements *result = SplitElements_from_lists(raw_result, separators);
    ElementsList_delete(raw_result);
    ElementList_delete(separators);
    return result;
}

struct SplitElements *SplitElements_by_operator(struct ElementQueue *queue, const char *text) {
    struct ElementsList *raw_result = ElementsList_create();
    struct ElementList *separators = ElementList_create();
    struct ElementList *group = ElementList_create();
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (is_operator_with_text(element, text)) {
            if (ElementList_size(group) == 0) {
                fail_at_position(element->position, "Unexpected token");
            }
            ElementsList_append(raw_result, Elements_from_list(group));
            ElementList_delete(group);
            group = ElementList_create();
            ElementList_append(separators, element);
        } else {
            ElementList_append(group, element);
        }
    }
    if (ElementList_size(group) > 0) {
        ElementsList_append(raw_result, Elements_from_list(group));
    }
    ElementList_delete(group);
    struct SplitElements *result = SplitElements_from_lists(raw_result, separators);
    ElementsList_delete(raw_result);
    ElementList_delete(separators);
    return result;
}

struct SplitElements *
SplitElements_by_predicate(struct ElementQueue *queue, bool (*predicate)(const struct Element *)) {
    struct ElementsList *raw_result = ElementsList_create();
    struct ElementList *separators = ElementList_create();
    struct ElementList *group = ElementList_create();
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (predicate(element)) {
            if (ElementList_size(group) == 0) {
                fail_at_position(element->position, "Unexpected token");
            }
            ElementsList_append(raw_result, Elements_from_list(group));
            ElementList_delete(group);
            group = ElementList_create();
            ElementList_append(separators, element);
        } else {
            ElementList_append(group, element);
        }
    }
    if (ElementList_size(group) > 0) {
        ElementsList_append(raw_result, Elements_from_list(group));
    }
    ElementList_delete(group);
    struct SplitElements *result = SplitElements_from_lists(raw_result, separators);
    ElementsList_delete(raw_result);
    ElementList_delete(separators);
    return result;
}

static bool is_possible_line_start(const struct Element *element) {
    return element->type == TokenElement && element->token->type == SymbolToken;
}

struct SplitElements *SplitElements_by_line(struct ElementQueue *queue) {
    struct ElementsList *raw_result = ElementsList_create();
    struct ElementList *group = ElementList_create();
    size_t current_line = 0;
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (!element) {
            break;
        }
        if (element->position.line != current_line) {
            if (is_possible_line_start(element)) {
                if (ElementList_size(group) > 0) {
                    ElementsList_append(raw_result, Elements_from_list(group));
                    ElementList_delete(group);
                    group = ElementList_create();
                }
            }
            current_line = element->position.line;
        }
        if (element->type == BracketElement) {
            current_line = element->bracket->closing_bracket->position.line;
        }
        ElementList_append(group, element);
    }
    if (ElementList_size(group) > 0) {
        ElementsList_append(raw_result, Elements_from_list(group));
    }
    ElementList_delete(group);
    struct SplitElements *result = SplitElements_from_lists(raw_result, 0);
    ElementsList_delete(raw_result);
    return result;
}
