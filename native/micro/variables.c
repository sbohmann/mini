#include "variables.h"

#include <collections/struct.h>

#include <minic/any.h>
#include <core/allocate.h>
#include <core/complex.h>
#include "debug.h"

struct Variables {
    struct ComplexValue base;
    struct Struct *scope;
    struct Variables *context;
};

static void Variables_destructor(struct Variables *instance) {
    Struct_release(instance->scope);
    if (instance->context) {
        Variables_release(instance->context);
    }
}

struct Variables *Variables_create(struct Variables *context) {
    struct Variables *result = allocate(sizeof(struct Variables));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *)) Variables_destructor;
    result->scope = Struct_create();
    result->context = context;
    if (context) {
        Variables_retain(context);
    }
    return result;
}

void Variables_retain(struct Variables *instance) {
    retain(&instance->base);
}

void Variables_release(struct Variables *instance) {
    release(&instance->base);
}

bool set_variable(struct Variables *self, const struct String *name, struct Any value) {
    bool result = Struct_set(self->scope, name, value);
    if (!result && self->context) {
        return set_variable(self->context, name, value);
    }
    return result;
}

void create_variable(struct Variables *self, const struct String *name, struct Any value) {
    // TODO implement and use put if absent and return a bool
    Struct_put(self->scope, name, value);
}

struct MapResult get_variable(const struct Variables *self, const struct String *name) {
    struct MapResult result = Struct_get(self->scope, name);
    if (!result.found && self->context) {
        return get_variable(self->context, name);
    }
    return result;
}
