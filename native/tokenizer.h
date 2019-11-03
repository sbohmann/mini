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

struct Tokens
{
    const char *path;
    const struct Source *source;
};

struct Tokens * read_file(const char *path);
