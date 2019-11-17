#pragma once

#include <stddef.h>
#include <stdbool.h>

struct String {
    size_t length;
    const char * value;
};

struct String *String_from_literal(const char *literal);

struct String *String_from_buffer(char *literal);

bool equal(const struct String *lhs, const char *rhs);

bool equals(const struct String *lhs, const struct String *rhs);
