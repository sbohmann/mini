#pragma once

#include <stdint.h>
#include <stdbool.h>

enum AnyType {
    NoneType = 0x00,
    IntegerType = 0x01,
    StringType = 0x11,
    ComplexType = 0x21,
    FlatType = 0x22,
    UndefinedType = 0xff
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

struct Any Undefined();

void Any_retain(struct Any instance);

void Any_release(struct Any instance);
