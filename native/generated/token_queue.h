#pragma once

#include "minic/tokens/token.h"

struct TokenQueue;

struct TokenQueue *TokenQueue_create(const struct Tokens *list);

void TokenQueue_delete(struct TokenQueue *instance);

const struct Token *TokenQueue_peek(struct TokenQueue *self);

const struct Token *TokenQueue_next(struct TokenQueue *self);

bool TokenQueue_contains(struct TokenQueue *self, bool (*predicate)(const struct Token *));
