#pragma once

#include "../tokenizer.h"

struct TokenList;

struct TokenListElement;

struct TokenList * TokenList_create();

void TokenList_delete(struct TokenList * list);

void TokenList_append(struct TokenList * list, struct Token *value);

void TokenList_prepend(struct TokenList * list, struct Token *value);

struct TokenListElement * TokenList_begin(struct TokenList * list);

struct TokenListElement * TokenList_end(struct TokenList * list);

struct TokenListElement * TokenListIterator_next(struct TokenListElement * iterator);

struct TokenListElement * TokenListIterator_previous(struct TokenListElement * iterator);

struct Token * TokenListIterator_get(struct TokenListElement * value);
