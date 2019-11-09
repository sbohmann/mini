#pragma once

#include <stddef.h>

#include "minic/tokenizer.h"

struct TokenList;

struct TokenListElement;

struct TokenList * TokenList_create();

struct TokenList * TokenList_delete(struct TokenList * instance);

size_t TokenList_size(struct TokenList * self);

void TokenList_append(struct TokenList * self, struct Token *value);

void TokenList_prepend(struct TokenList * self, struct Token *value);

struct Token * TokenListIterator_to_array(struct TokenList * self);

struct TokenListElement * TokenList_begin(struct TokenList * self);

struct TokenListElement * TokenList_end(struct TokenList * self);

struct TokenListElement * TokenListIterator_next(struct TokenListElement * iterator);

struct TokenListElement * TokenListIterator_previous(struct TokenListElement * iterator);

struct Token * TokenListIterator_get(struct TokenListElement * value);
