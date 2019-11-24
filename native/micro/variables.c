#include "variables.h"

#include <collections/hashmap.h>

#include <minic/any.h>
#include <core/allocate.h>
#include <stdlib.h>
#include <core/complex.h>
#include "debug.h"

struct Variables {
    struct ComplexValue base;
    struct HashMap *scope;
    struct Variables *context;
};

struct Variables *Variables_create(struct Variables *context) {
    struct Variables *result = allocate(sizeof(struct Variables));
    Complex_init(&result->base);
    result->scope = HashMap_create();
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
    struct HashMap *scope = instance->scope;
    struct Variables *context = instance->context;
    if (release(&instance->base)) {
        HashMap_delete(scope);
        if (context) {
            Variables_release(context);
        }
    }
}

bool set_variable(struct Variables *self, const struct String *name, struct Any value) {
    bool result = HashMap_set(self->scope, String(name), value);
    if (!result && self->context) {
        return set_variable(self->context, name, value);
    }
    return result;
}

void create_variable(struct Variables *self, const struct String *name, struct Any value) {
    // TODO implement and use put if absent and return a bool
    HashMap_put(self->scope, String(name), value);
}

struct HashMapResult get_variable(struct Variables *self, const struct String *name) {
    struct HashMapResult result = HashMap_get(self->scope, String(name));
    if (!result.found && self->context) {
        return get_variable(self->context, name);
    }
    return result;
}
