#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "core/string.h"

enum AnyType {
    NoneType = 0x00,
    IntegerType = 0x01,
    StringType = 0x11,
    ComplexType = 0x21,
    FlatType = 0x22
};

struct Any {
    enum AnyType type;
    union {
        char flat_value[8];
        int64_t integer;
        const struct String *string;
        struct ComplexValue * complex_value;
    };
};

struct Any None();

struct Any String(const struct String *value);

struct Any Integer(int64_t value);

struct Any Complex(struct ComplexValue *instance);

void Any_retain(struct Any instance);

void Any_release(struct Any instance);

Hash int64_hash(int64_t value);

Hash uint64_hash(uint64_t value);

Hash string_hash(const char *data, size_t length);

Hash Any_hash(struct Any value);

bool Any_equal(struct Any lhs, struct Any rhs);
