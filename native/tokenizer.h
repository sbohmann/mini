#pragma once

#include <stddef.h>

#include "string.h"
#include "source.h"
#include "types.h"

struct Position {
    const char *path;
    const size_t line;
    const size_t column;
};

struct Token {
    const char *text;
    const struct Any value;
    const struct Position position;
};

struct Tokens {
    const size_t size;
    const struct Token *data;
};

struct ParsedModule
{
    const char *path;
    const struct Source *source;
    const size_t tokens_size;
    const struct Tokens *tokens;
};

struct ParsedModule * read_file(const char *path);
