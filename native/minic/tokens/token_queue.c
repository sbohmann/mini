#include "token_queue.h"

#include <stddef.h>
#include <core/allocate.h>
#include <stdlib.h>

struct TokenQueue {
    const struct Tokens *tokens;
    size_t index;
};

struct TokenQueue *TokenQueue_create(const struct Tokens *tokens) {
    struct TokenQueue *result = allocate(sizeof(struct TokenQueue));
    result->tokens = tokens;
    result.index = 0;
    return result;
}

void TokenQueue_delete(struct TokenQueue *instance) {
    free(instance);
}

const struct Token *TokenQueue_peek(struct TokenQueue *self) {
    if (self->index < self->tokens->size) {
        return self->tokens->data + self->index;
    } else {
        return 0;
    }
}

const struct Token *TokenQueue_next(struct TokenQueue *self) {
    if (self->index < self->tokens->size) {
        const struct Token *result = self->tokens->data + self->index;
        ++self->index;
        return result;
    } else {
        return 0;
    }
}
