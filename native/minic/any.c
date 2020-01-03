#include "any.h"

#include <string.h>
#include <core/complex.h>
#include <core/errors.h>
#include <core/allocate.h>

static const size_t reasonable_prime = 92821;

const char *AnyType_to_string(enum AnyType type) {
    switch (type) {
        case NoneType:
            return "None";
        case IntegerType:
            return "Integer";
        case StringType:
            return "String";
        case ComplexType:
            return "Complex";
        case FlatType:
            return "Flat";
        case FunctionPointerType:
            return "FunctionPointer";
        default:
            fail("<Unknown type %d>", type);
    }
}

struct Any None() {
    static struct Any *result = 0;
    if (result == 0) {
        result = allocate(sizeof(struct Any));
    }
    return *result;
}

struct Any Boolean(bool value) {
    struct Any result = None();
    result.type = BooleanType;
    result.boolean = value;
    return result;
}

struct Any True() {
    struct Any result = None();
    result.type = BooleanType;
    result.boolean = true;
    return result;
}

struct Any False() {
    struct Any result = None();
    result.type = BooleanType;
    result.boolean = false;
    return result;
}

struct Any Not(struct Any value) {
    return Boolean(!Any_true(value));
}

struct Any Integer(int64_t value) {
    struct Any result = None();
    result.type = IntegerType;
    result.integer = value;
    return result;
}

struct Any String(const struct String *value) {
    struct Any result = None();
    result.type = StringType;
    result.string = value;
    return result;
}

struct Any Function(struct Any (*value) (const struct List *)) {
    struct Any result = None();
    result.type = FunctionPointerType;
    result.function = value;
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
            // TODO relay
            return (size_t) value.complex_value;
        case FlatType:
            return string_hash(value.flat_value, sizeof(value.flat_value));
        case FunctionPointerType:
            return (size_t) value.function;
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
            return true;
        case IntegerType:
            return lhs.integer == rhs.integer;
        case StringType:
            return String_equal(lhs.string, rhs.string);
        case ComplexType:
            // TODO relay
            return lhs.complex_value == rhs.complex_value;
        case FlatType:
            return lhs.flat_value == rhs.flat_value;
        case FunctionPointerType:
            return lhs.function == rhs.function;
        default:
            fail("Any_equal: value has unknown type %d", type);
    }
}

bool Any_unequal(struct Any lhs, struct Any rhs) {
    return !Any_equal(lhs, rhs);
}

bool Any_less_than(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        return lhs.integer < rhs.integer;
    } else {
        fail("Comparison not supported for types %s and %s", Any_typename(lhs), Any_typename(rhs));
    }
}

bool Any_greater_than(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        return lhs.integer > rhs.integer;
    } else {
        fail("Comparison not supported for types %s and %s", Any_typename(lhs), Any_typename(rhs));
    }
}

bool Any_less_than_or_equal(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        return lhs.integer <= rhs.integer;
    } else {
        fail("Comparison not supported for types %s and %s", Any_typename(lhs), Any_typename(rhs));
    }
}

bool Any_greater_than_or_equal(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        return lhs.integer >= rhs.integer;
    } else {
        fail("Comparison not supported for types %s and %s", Any_typename(lhs), Any_typename(rhs));
    }
}

bool Any_true(struct Any value) {
    switch (value.type) {
        case NoneType:
            return false;
        case BooleanType:
            return value.boolean;
        case StringType:
            // true because equivalent to a ComplexString
            return true;
        case ComplexType:
            // Complex types are never null
            return true;
        default:
            fail("Cannot convert type %s to boolean", Any_typename(value));
    }
}

struct Any Any_add(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        struct Any result = Integer(lhs.integer + rhs.integer);
        return result;
    } else {
        fail("Addition unsupported for types %s and %s: ", Any_typename(lhs), Any_typename(rhs));
    }
}

struct Any Any_subtract(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        struct Any result = Integer(lhs.integer - rhs.integer);
        return result;
    } else {
        fail("Subtraction unsupported for types %s and %s: ", Any_typename(lhs), Any_typename(rhs));
    }
}

struct Any Any_multiply(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        struct Any result = Integer(lhs.integer * rhs.integer);
        return result;
    } else {
        fail("Multiplication unsupported for types %s and %s: ", Any_typename(lhs), Any_typename(rhs));
    }
}

struct Any Any_divide(struct Any lhs, struct Any rhs) {
    if (lhs.type == IntegerType && rhs.type == IntegerType) {
        if (rhs.integer == 0) {
            fail("Division by zero", Any_typename(lhs), Any_typename(rhs));
        }
        struct Any result = Integer(lhs.integer / rhs.integer);
        return result;
    } else {
        fail("Division unsupported for types %s and %s: ", Any_typename(lhs), Any_typename(rhs));
    }
}

const char *Any_typename(struct Any value) {
    if (value.type == ComplexType) {
        return Complex_typename(value.complex_value);
    } else {
        return AnyType_to_string(value.type);
    }
}
