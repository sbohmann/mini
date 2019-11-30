#include "{{file}}.h"

#include <stddef.h>
#include <core/allocate.h>
#include <stdlib.h>

struct {{name}}Queue {
    const struct {{name}}s *list;
    size_t index;
};

struct {{name}}Queue *{{name}}Queue_create(const struct {{name}}s *list) {
    struct {{name}}Queue *result = allocate(sizeof(struct {{name}}Queue));
    result->list = list;
    result->index = 0;
    return result;
}

void {{name}}Queue_delete(struct {{name}}Queue *instance) {
    free(instance);
}

{{constvalue}}{{name}}Queue_peek(struct {{name}}Queue *self) {
    if (self->index < self->list->size) {
        return self->list->data + self->index;
    } else {
        return 0;
    }
}

{{constvalue}}{{name}}Queue_next(struct {{name}}Queue *self) {
    if (self->index < self->list->size) {
        {{constvalue}}result = self->list->data + self->index;
        ++self->index;
        return result;
    } else {
        return 0;
    }
}

bool {{name}}Queue_contains(struct {{name}}Queue *self, bool (*predicate)(const struct Element *)) {
    size_t size = self->list->size;
    for (size_t index = self->index; index < size; ++index) {
        if (predicate(&self->list->data[index])) {
            return true;
        }
    }
    return false;
}

