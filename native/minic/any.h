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

struct Variable {
    const struct String *name;
    struct Any value;
};

struct Any None();

struct Any String(const char *value);

struct Any Integer(int64_t value);

void Any_retain(struct Any instance);

void Any_release(struct Any instance);

uint32_t int64_hash(int64_t value);

uint32_t uint64_hash(uint64_t value);

uint32_t string_hash(const char *data, size_t length);

uint32_t Any_hash(struct Any value);

bool Any_equal(struct Any lhs, struct Any rhs);
