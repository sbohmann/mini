#pragma once

#include <stdbool.h>
#include <stdint-gcc.h>

struct ReferenceCount;

enum ComplexType {
    StringComplexType = 0x11,
    ListComplexType = 0x21,
    SetComplexType = 0x22,
    MapComplexType = 0x23,
    StructComplexType = 0x24
};

struct ComplexValue {
    struct ReferenceCount *reference_count;
    void (*destructor)(struct ComplexValue *);
    uint8_t type;
};

void Complex_init(struct ComplexValue *instance);

void retain(struct ComplexValue *instance);
bool release(struct ComplexValue *instance);
