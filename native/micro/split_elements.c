#include "split_elements.h"

#include <stdlib.h>

#include <generated/element_list.h>
#include <generated/element_queue.h>
#include <minic/expressions/expressions.h>
#include <generated/elements_list.h>
#include <core/allocate.h>

struct SplitElements *SplitElements_from_list(const struct ElementsList *list) {
    struct SplitElements *result = allocate(sizeof(struct SplitElements));
    result->size = ElementsList_size(list);
    result->data = ElementsList_to_array(list);
    return result;
}

void SplitElements_delete(struct SplitElements *instance) {
    free(instance->data);
    free(instance);
}

bool is_comma(const struct Element *element) {
    return element->type == TokenElement &&
           element->token->type == Operator &&
           equal(element->token->text, ",");
}

struct SplitElements *SplitElements_by_comma(struct ElementQueue *queue) {
    struct ElementsList *raw_result = ElementsList_create();
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
        } else {
            ElementList_append(group, element);
        }
    }
    if (ElementList_size(group) > 0) {
        ElementsList_append(raw_result, Elements_from_list(group));
    }
    ElementList_delete(group);
    struct SplitElements *result = SplitElements_from_list(raw_result);
    ElementsList_delete(raw_result);
    return result;
}