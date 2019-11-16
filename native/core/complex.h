#pragma once

#include <stdbool.h>

struct ReferenceCount;

struct ComplexValue {
    struct ReferenceCount *reference_count;
};

void retain(struct ComplexValue *instance);
bool release(struct ComplexValue *instance);
