#include "complex.h"
#include "errors.h"
#include "allocate.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>

struct ReferenceCount {
    atomic_size_t strong_count;
    atomic_size_t weak_count;
};

void Complex_init(struct ComplexValue *instance) {
    instance->reference_count = allocate(sizeof(struct ReferenceCount));
    instance->reference_count->weak_count = 1;
    instance->reference_count->strong_count = 1;
}

void retain(struct ComplexValue *instance) {
    if (!instance) {
        fail("Attempting to retain a null ComplexValue pointer");
    }
    ++instance->reference_count->weak_count;
    ++instance->reference_count->strong_count;
}

bool release(struct ComplexValue *instance) {
    if (!instance) {
        fail("Attempting to release a null ComplexValue pointer");
    }
    size_t new_strong_count = --instance->reference_count->strong_count;
    size_t new_weak_count = --instance->reference_count->weak_count;
    if (new_weak_count == 0) {
        free(instance->reference_count);
        if (new_strong_count != 0) {
            fail("weak count went to zero but strong count went to %zu", new_strong_count);
        }
        free(instance);
        return true;
    } else if (new_strong_count == 0) {
        free(instance);
        return true;
    }
    return false;
}
