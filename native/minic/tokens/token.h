#pragma once

#include <stddef.h>

#include "core/string.h"
#include "minic/source.h"
#include "minic/any.h"

struct Position {
    const char *path;
    size_t line;
    size_t column;
};

enum TokenType {
    SymbolToken,
    StringLiteralToken,
    NumberLiteralToken,
    OperatorToken,
    OpeningBracketToken,
    ClosingBracketToken
};

struct Token {
    enum TokenType type;
    const struct String *text;
    struct Any value;
    struct Position position;
};

struct Tokens {
    size_t size;
    const struct Token *data;
};

struct TokenQueue;

struct Tokens *read_tokens(const char *path, const struct Source *source);

_Noreturn void fail_at_position(struct Position position, const char *format, ...);

_Noreturn void fail_after_position(struct Position position, const char *format, ...);
