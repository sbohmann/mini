#include "token_queue.h"

#include <stddef.h>
#include <core/allocate.h>
#include <stdlib.h>

struct TokenQueue {
    const struct Tokens *list;
    size_t index;
};

struct TokenQueue *TokenQueue_create(const struct Tokens *list) {
    struct TokenQueue *result = allocate(sizeof(struct TokenQueue));
    result->list = list;
    result->index = 0;
    return result;
}

void TokenQueue_delete(struct TokenQueue *instance) {
    free(instance);
}

const struct Token *TokenQueue_peek(struct TokenQueue *self) {
    if (self->index < self->list->size) {
        return self->list->data + self->index;
    } else {
        return 0;
    }
}

const struct Token *TokenQueue_next(struct TokenQueue *self) {
    if (self->index < self->list->size) {
        const struct Token *result = self->list->data + self->index;
        ++self->index;
        return result;
    } else {
        return 0;
    }
}