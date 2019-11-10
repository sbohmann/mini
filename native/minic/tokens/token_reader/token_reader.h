#pragma once

#include "minic/tokens/token.h"

struct TokenReader;

struct TokenReader *TokenReader_create(struct Position position, char initial_char, char next_char);

void TokenReader_delete(struct TokenReader *instance);

bool TokenReader_add_char(struct TokenReader *self, char c);

struct Token *TokenReader_result(struct TokenReader *self);
