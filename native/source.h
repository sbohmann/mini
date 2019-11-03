#pragma once

#include <stddef.h>

#include "string.h"

struct Source {
    size_t number_of_lines;
    const struct String * lines;
};

struct Source * read_source(const char *path);
