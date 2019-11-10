#pragma once

#include <stddef.h>

#include "core/string.h"
#include "source.h"
#include "types.h"

struct Position {
    const char *path;
    size_t line;
    size_t column;
};

struct Token {
    const struct String *text;
    struct Any value;
    struct Position position;
};

struct Tokens {
    size_t size;
    const struct Token *data;
};

struct ParsedModule
{
    const char *path;
    const struct Source *source;
    const struct Tokens *tokens;
};

struct ParsedModule * read_file(const char *path);

void fail_at_position(struct Position position, const char *format, ...);
