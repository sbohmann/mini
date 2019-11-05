#pragma once

struct TokenList;

struct TokenListElement;

TokenList * TokenList_create();

TokenList * TokenList_delete(struct TokenList * list);

void TokenList_append(struct TokenList * list, struct Token *value);

void TokenList_prepend(struct TokenList * list, struct Token *value);

TokenListElement * TokenList_begin(struct TokenList * list);

TokenListElement * TokenList_end(struct TokenList * list);

TokenListElement * TokenListIterator_next(TokenListElement * iterator);

TokenListElement * TokenListIterator_previous(TokenListElement * iterator);

struct Token * TokenListIterator_get(TokenListElement * value);
