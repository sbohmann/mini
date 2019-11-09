#pragma once

struct ReferenceCount;

struct ComplexValue {
    struct ReferenceCount *reference_count;
};

void retain(struct ComplexValue *instance);
void release(struct ComplexValue *instance);

struct Any {
    char complex;
    union {
        char primitive_value[8];
        struct ComplexValue * complex_value;
    };
};
