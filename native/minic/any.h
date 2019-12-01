#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "core/string.h"

enum AnyType {
    NoneType = 0x00,
    BooleanType = 0x01,
    IntegerType = 0x02,
    StringType = 0x11,
    ComplexType = 0x21,
    FlatType = 0x22
};

struct Any {
    enum AnyType type;
    union {
        char flat_value[8];
        bool boolean;
        int64_t integer;
        const struct String *string;
        struct ComplexValue * complex_value;
    };
};

struct Any None();

struct Any Boolean(bool value);

struct Any True();

struct Any False();

struct Any Not(struct Any value);

struct Any Integer(int64_t value);

struct Any String(const struct String *value);

struct Any Complex(struct ComplexValue *instance);

void Any_retain(struct Any instance);

void Any_release(struct Any instance);

Hash int64_hash(int64_t value);

Hash uint64_hash(uint64_t value);

Hash string_hash(const char *data, size_t length);

Hash Any_hash(struct Any value);

struct Any Any_equal(struct Any lhs, struct Any rhs);

struct Any Any_unequal(struct Any lhs, struct Any rhs);

struct Any Any_less_than(struct Any lhs, struct Any rhs);

struct Any Any_greater_than(struct Any lhs, struct Any rhs);

struct Any Any_less_than_or_equal(struct Any lhs, struct Any rhs);

struct Any Any_greater_than_or_equal(struct Any lhs, struct Any rhs);

struct Any Any_true(struct Any value);

struct Any Any_add(const struct Any lhs, const struct Any rhs);

struct Any Any_subtract(const struct Any lhs, const struct Any rhs);

struct Any Any_multiply(const struct Any lhs, const struct Any rhs);

struct Any Any_divide(const struct Any lhs, const struct Any rhs);

const char *Any_typename(struct Any value);
