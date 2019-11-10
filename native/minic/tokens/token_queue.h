#pragma once

#include "token.h"

struct TokenQueue;

struct TokenQueue *TokenQueue_create(const struct Tokens *tokens);

void TokenQueue_delete(struct TokenQueue *instance);

const struct Token *TokenQueue_peek(struct TokenQueue *instance);

const struct Token *TokenQueue_next(struct TokenQueue *instance);
