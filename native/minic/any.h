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
    FlatType = 0x22,
    FunctionPointerType = 0x23
};

struct List;

struct Any {
    enum AnyType type;
    union {
        bool boolean;
        int64_t integer;
        const struct String *string;
        struct ComplexValue * complex_value;
        char flat_value[8];
        struct Any (*function)(const struct List *);
    };
};

struct Any None();

struct Any Boolean(bool value);

struct Any True();

struct Any False();

struct Any Not(struct Any value);

struct Any Integer(int64_t value);

struct Any String(const struct String *value);

struct Any Function(struct Any (*value) (const struct List *));

struct Any Complex(struct ComplexValue *instance);

void Any_retain(struct Any instance);

void Any_release(struct Any instance);

Hash int64_hash(int64_t value);

Hash uint64_hash(uint64_t value);

Hash string_hash(const char *data, size_t length);

Hash Any_hash(struct Any value);

bool Any_equal(struct Any lhs, struct Any rhs);

bool Any_unequal(struct Any lhs, struct Any rhs);

bool Any_less_than(struct Any lhs, struct Any rhs);

bool Any_greater_than(struct Any lhs, struct Any rhs);

bool Any_less_than_or_equal(struct Any lhs, struct Any rhs);

bool Any_greater_than_or_equal(struct Any lhs, struct Any rhs);

bool Any_true(struct Any value);

struct Any Any_add(struct Any lhs, struct Any rhs);

struct Any Any_subtract(struct Any lhs, struct Any rhs);

struct Any Any_multiply(struct Any lhs, struct Any rhs);

struct Any Any_divide(struct Any lhs, struct Any rhs);

const char *Any_typename(struct Any value);
