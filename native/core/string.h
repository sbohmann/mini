#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "typedefs.h"

struct String {
    size_t length;
    const char * value;
    Hash hash;
    bool is_literal;
};

const struct String *String_from_literal(const char *literal);

const struct String *String_preallocated(const char *value, size_t length);

const struct String *String_from_buffer(char *literal, size_t length);

bool String_equal(const struct String *lhs, const struct String *rhs);

bool String_equal_to_literal(const struct String *lhs, const char *rhs);

void String_pin(const struct String *instance);

void String_delete(struct String *instance);
