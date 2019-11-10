#pragma once

#include <stddef.h>
#include <stdbool.h>

struct String {
    size_t length;
    const char * value;
};

bool equal(const struct String *lhs, const char *rhs);
