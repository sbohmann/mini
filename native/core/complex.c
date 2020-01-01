#include "complex.h"
#include "errors.h"
#include "allocate.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>
#include <micro/debug.h>

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
    if (DEBUG_ENABLED) fprintf(stderr, "retain from %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    ++instance->reference_count->weak_count;
    ++instance->reference_count->strong_count;
    if (DEBUG_ENABLED) fprintf(stderr, "retained to %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
}

bool release(struct ComplexValue *instance) {
    if (!instance) {
        fail("Attempting to release a null ComplexValue pointer");
    }
    if (DEBUG_ENABLED) fprintf(stderr, "release from %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    size_t new_strong_count = --instance->reference_count->strong_count;
    size_t new_weak_count = --instance->reference_count->weak_count;
    if (DEBUG_ENABLED) fprintf(stderr, "released to %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    if (new_weak_count == 0) {
        if (DEBUG_ENABLED) fprintf(stderr, "both zero\n");
        free(instance->reference_count);
        if (new_strong_count != 0) {
            fail("weak count went to zero but strong count went to %zu\n", new_strong_count);
        }
        if (instance->destructor) {
            instance->destructor(instance);
        }
        free(instance);
        return true;
    } else if (new_strong_count == 0) {
        if (DEBUG_ENABLED) fprintf(stderr, "strong zero\n");
        if (instance->destructor) {
            instance->destructor(instance);
        }
        free(instance);
        return true;
    }
    return false;
}

const char *Complex_typename(struct ComplexValue *value) {
    switch (value->type) {
        case StringComplexType:
            return "String";
        case ListComplexType:
            return "List";
        case SetComplexType:
            return "HashSet";
        case MapComplexType:
            return "HashMap";
        case StructComplexType:
            return "Struct";
        case FunctionComplexType:
            return "Function";
        default:
            fail("<unknown complex type %d>", value->type);
    }
}
