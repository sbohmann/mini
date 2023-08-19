#include "element_queue.h"
#include "core/errors.h"

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
    deallocate(instance);
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

const struct Element *ElementQueue_take(struct ElementQueue *self) {
    if (self->index < self->list->size) {
        const struct Element *result = self->list->data + self->index;
        ++self->index;
        return result;
    } else {
        fail_after_position(ElementQueue_last_position(self), "Unexpected end of input");
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

void ElementQueue_reset(struct ElementQueue *self) {
    self->index = 0;
}

struct Position ElementQueue_last_position(struct ElementQueue *self) {
    if (self->list->size > 0) {
        return self->list->data[self->list->size - 1].position;
    } else {
        return (struct Position) { "", 0, 0 };
    }
}
