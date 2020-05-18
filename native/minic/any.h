#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "core/string.h"

enum AnyType {
    NoneType = 0x00,
    BooleanType = 0x01,
    IntegerType = 0x02,
    StringLiteralType = 0x11,
    ComplexType = 0x21,
    FlatType = 0x22,
    FunctionPointerType = 0x23,
    ErrorType = 0x30
};

const char *AnyType_to_string(enum AnyType type);

struct List;

struct Any {
    enum AnyType type;
    union {
        bool boolean;
        int64_t integer;
        const struct String *string;
        struct ComplexValue * complex_value;
        char flat_value[8];
        struct {
            struct Any (*pointer)(const struct List *);
            const struct String *name;
        } function;
    };
};

struct Any None(void);

struct Any Boolean(bool value);

struct Any True(void);

struct Any False(void);

struct Any Not(struct Any value);

struct Any Integer(int64_t value);

struct Any StringLiteral(const struct String *value);

struct Any FunctionPointer(struct Any (*value) (const struct List *), const char *name);

struct Any Complex(struct ComplexValue *instance);

struct Any Error(const char *format, ...);

void Any_retain(struct Any instance);

void Any_release(struct Any instance);

Hash int64_hash(int64_t value);

Hash uint64_hash(uint64_t value);

Hash string_hash(const char *data, size_t length);

Hash Any_hash(struct Any value);

bool Any_raw_equal(struct Any lhs, struct Any rhs);

struct Any Any_equal(struct Any lhs, struct Any rhs);

struct Any Any_unequal(struct Any lhs, struct Any rhs);

struct Any Any_less_than(struct Any lhs, struct Any rhs);

struct Any Any_greater_than(struct Any lhs, struct Any rhs);

struct Any Any_less_than_or_equal(struct Any lhs, struct Any rhs);

struct Any Any_greater_than_or_equal(struct Any lhs, struct Any rhs);

bool Any_raw_true(struct Any value);

struct Any Any_true(struct Any value);

struct Any Any_add(struct Any lhs, struct Any rhs);

struct Any Any_subtract(struct Any lhs, struct Any rhs);

struct Any Any_multiply(struct Any lhs, struct Any rhs);

struct Any Any_divide(struct Any lhs, struct Any rhs);

const char *Any_typename(struct Any value);
