#pragma once

#include <stdbool.h>
#include <stdint.h>

struct ReferenceCount;

enum ComplexType {
    StringComplexType = 0x11,
    ListComplexType = 0x21,
    SetComplexType = 0x22,
    MapComplexType = 0x23,
    StructComplexType = 0x24,
    FunctionComplexType = 0x81
};

const char *ComplexType_to_string(enum ComplexType type);

struct ComplexValue {
    struct ReferenceCount *reference_count;
    void (*destructor)(struct ComplexValue *);
    enum ComplexType type;
};

void Complex_init(struct ComplexValue *instance);

void retain(struct ComplexValue *instance);
bool release(struct ComplexValue *instance);

const char *Complex_typename(struct ComplexValue *value);
