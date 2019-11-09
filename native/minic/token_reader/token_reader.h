#pragma once

struct TokenReader;

struct TokenReader *TokenReader_create(char initial_char);

void TokenReader_delete(struct TokenReader *instance);

bool TokenReader_add_char(struct TokenReader *self, char c);

struct Token *TokenReader_result(struct TokenReader *self);
