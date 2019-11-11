#pragma once

#include <stdint.h>
#include <stdbool.h>

struct ReferenceCount;

struct ComplexValue {
    struct ReferenceCount *reference_count;
};

void retain(struct ComplexValue *instance);
void release(struct ComplexValue *instance);

enum AnyType {
    None = 0x00,
    Integer = 0x01,
    String = 0x11,
    Complex = 0x21,
    Flat = 0x22
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

struct Any Any_create();
