#pragma once

#include <stdint.h>
#include <core/complex.h>
#include "variables.h"

struct Function {
    struct ComplexValue base;
    struct Variables *bindings;
    const struct StringList *parameter_names;
    const struct Elements *body;
};

void Function_destructor(struct Function *instance);

struct Function *Function_create(struct Variables *bindings, const struct StringList *parameter_names,
                                 const struct Elements *body);
