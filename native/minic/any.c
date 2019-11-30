#include "any.h"

#include <string.h>
#include <core/complex.h>
#include <core/errors.h>
#include <core/allocate.h>

static const size_t reasonable_prime = 92821;

struct Any None() {
    static struct Any *result = 0;
    if (result == 0) {
        result = allocate(sizeof(struct Any));
    }
    return *result;
}

struct Any String(const struct String *value) {
    struct Any result = None();
    result.type = StringType;
    result.string = value;
    return result;
}

struct Any Integer(int64_t value) {
    struct Any result = None();
    result.type = IntegerType;
    result.integer = value;
    return result;
}

struct Any Complex(struct ComplexValue *instance) {
    struct Any result = None();
    result.type = ComplexType;
    result.complex_value = instance;
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

Hash int64_hash(int64_t value) {
    return uint64_hash(value);
}

Hash uint64_hash(uint64_t value) {
    return (size_t) (value ^ (value >> 32u));
}

Hash string_hash(const char *data, size_t length) {
    Hash result = uint64_hash(0x898cc428c6d364b8);
    for (size_t index = 0; index < length; ++index) {
        result *= reasonable_prime;
        result += data[index];
    }
    return result;
}

Hash Any_hash(struct Any value) {
    switch (value.type) {
        case NoneType:
            return 0;
        case IntegerType:
            return int64_hash(value.integer);
        case StringType:
            return value.string->hash;
        case ComplexType:
            return (size_t) value.complex_value;
        case FlatType:
            return string_hash(value.flat_value, sizeof(value.flat_value));
        default:
            fail("Any_hash: value has unknown type %d", value.type);
    }
}

bool Any_equal(struct Any lhs, struct Any rhs) {
    enum AnyType type = lhs.type;
    if (rhs.type != type) {
        return false;
    }
    switch (type) {
        case NoneType:
            return 0;
        case IntegerType:
            return lhs.integer == rhs.integer;
        case StringType:
            return String_equal(lhs.string, rhs.string);
        case ComplexType:
            // TODO relay
            return lhs.complex_value == rhs.complex_value;
        case FlatType:
            return lhs.flat_value == rhs.flat_value;
        default:
            fail("Any_equal: value has unknown type %d", type);
    }
}
