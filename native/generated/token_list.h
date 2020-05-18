#pragma once

#include <stddef.h>

#include "minic/tokens/token.h"

struct TokenList;

struct TokenListElement;

struct TokenList * TokenList_create(void);

void TokenList_delete(struct TokenList * instance);

size_t TokenList_size(const struct TokenList * self);

void TokenList_append(struct TokenList * self, const struct Token *value);

void TokenList_prepend(struct TokenList * self, const struct Token *value);

struct Token * TokenList_to_array(const struct TokenList * self);

struct TokenListElement * TokenList_begin(const struct TokenList * self);

struct TokenListElement * TokenList_end(const struct TokenList * self);

struct TokenListElement * TokenListIterator_next(struct TokenListElement * iterator);

struct TokenListElement * TokenListIterator_previous(struct TokenListElement * iterator);

const struct Token * TokenListIterator_get(struct TokenListElement * value);
