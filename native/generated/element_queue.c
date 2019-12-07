#include "element_queue.h"

#include <stddef.h>
#include <core/allocate.h>
#include <stdlib.h>

struct ElementQueue {
    const struct Elements *list;
    size_t index;
};

struct ElementQueue *ElementQueue_create(const struct Elements *list) {
    struct ElementQueue *result = allocate(sizeof(struct ElementQueue));
    result->list = list;
    result->index = 0;
    return result;
}

void ElementQueue_delete(struct ElementQueue *instance) {
    free(instance);
}

const struct Element *ElementQueue_peek(struct ElementQueue *self) {
    if (self->index < self->list->size) {
        return self->list->data + self->index;
    } else {
        return 0;
    }
}

const struct Element *ElementQueue_next(struct ElementQueue *self) {
    if (self->index < self->list->size) {
        const struct Element *result = self->list->data + self->index;
        ++self->index;
        return result;
    } else {
        return 0;
    }
}

bool ElementQueue_contains(struct ElementQueue *self, bool (*predicate)(const struct Element *)) {
    size_t size = self->list->size;
    for (size_t index = self->index; index < size; ++index) {
        if (predicate(self->list->data + index)) {
            return true;
        }
    }
    return false;
}
