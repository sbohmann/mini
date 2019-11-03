#include "types.h"

#include <stddef.h>
#include <stdatomic.h>
#include <stdlib.h>

struct ReferenceCount {
    atomic_size_t strong_count;
    atomic_size_t weak_count;
};

void retain(struct ComplexValue *instance) {
    ++instance->reference_count->weak_count;
    ++instance->reference_count->strong_count;
}

void release(struct ComplexValue *instance) {
    size_t new_strong_count = --instance->reference_count->strong_count;
    size_t new_weak_count = --instance->reference_count->weak_count;
    if (new_weak_count == 0) {
        free(instance->reference_count);
    }
    if (new_strong_count == 0) {
        free(instance);
    }
}
