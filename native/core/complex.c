#include "complex.h"
#include "errors.h"
#include "allocate.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <stdbool.h>
#include <micro/debug.h>

const char *ComplexType_to_string(enum ComplexType type) {
    switch (type) {
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
            fail_with_message("<unknown complex type %d>", type);
    }
}

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
        fail_with_message("Attempting to retain a null ComplexValue pointer");
    }
    if (DEBUG_ENABLED) fprintf(stderr, "retain from %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    ++instance->reference_count->weak_count;
    ++instance->reference_count->strong_count;
    if (DEBUG_ENABLED) fprintf(stderr, "retained to %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
}

bool release(struct ComplexValue *instance) {
    if (!instance) {
        fail_with_message("Attempting to release a null ComplexValue pointer");
    }
    if (DEBUG_ENABLED) fprintf(stderr, "release from %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    size_t new_strong_count = --instance->reference_count->strong_count;
    size_t new_weak_count = --instance->reference_count->weak_count;
    if (DEBUG_ENABLED) fprintf(stderr, "released to %zu, %zu\n", instance->reference_count->strong_count, instance->reference_count->weak_count);
    if (new_weak_count == 0) {
        if (DEBUG_ENABLED) fprintf(stderr, "both zero\n");
        deallocate(instance->reference_count);
        if (new_strong_count != 0) {
            fail_with_message("weak count went to zero but strong count went to %zu\n", new_strong_count);
        }
        if (instance->destructor) {
            instance->destructor(instance);
        }
        deallocate(instance);
        return true;
    } else if (new_strong_count == 0) {
        if (DEBUG_ENABLED) fprintf(stderr, "strong zero\n");
        if (instance->destructor) {
            instance->destructor(instance);
        }
        deallocate(instance);
        return true;
    }
    return false;
}

const char *Complex_typename(struct ComplexValue *value) {
    return ComplexType_to_string(value->type);
}
