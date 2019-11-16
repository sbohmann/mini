#include "types.h"

#include <string.h>
#include <core/complex.h>
#include <core/errors.h>
#include <core/allocate.h>

struct Any None() {
    static struct Any *result = 0;
    if (result == 0) {
        result = allocate(sizeof(struct Any));
    }
    return *result;
}

struct Any Undefined() {
    struct Any result = None();
    result.type = UndefinedType;
    return result;
}

void Any_retain(struct Any instance) {
    if (instance.type == ComplexType) {
        retain(instance.complex_value);
    }
}

void Any_release(struct Any instance) {
    if (instance.type == ComplexType) {
        if (release(instance.complex_value)) {
            memset(&instance, 0, sizeof(struct Any));
            if (instance.type != NoneType) {
                fail("Logical error - any type not None after memset: %d", instance.type);
            }
        }
    }
}
