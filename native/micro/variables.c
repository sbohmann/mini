#include "variables.h"

#include <collections/hashmap.h>

#include <minic/any.h>
#include <core/errors.h>
#include "debug.h"

struct HashMap *variables = 0;

void set_variable(const struct String *name, const struct String *text, struct Any value) {
    DEBUG("Setting variable %s to [%s]\n", name->value, text->value);
    HashMap_put(variables, String(name), value);
}

struct Any get_variable(const struct String *name) {
    DEBUG("Getting variable %s\n", name->value);
    return HashMap_get(variables, String(name));
}

void initialize_variables() {
    if (variables != 0) {
        fail("variables already initialized");
    }
    variables = HashMap_create();
}

void release_variables() {
    if (variables == 0) {
        fail("variables not initialized");
    }
    HashMap_delete(variables);
    variables = 0;
}
