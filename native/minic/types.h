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
    Flat = 0x00,
    Integer = 0x01,
    String = 0x02,
    Complex = 0x20
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

struct Any Any_create();
