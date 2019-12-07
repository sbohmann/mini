#include "function.h"

#include <core/allocate.h>
#include "variables.h"
#include "complex_types.h"

void Function_destructor(struct Function *instance) {
    if (instance->bindings) {
        Variables_release(instance->bindings);
    }
}

struct Function *Function_create(struct Variables *bindings, const struct StringList *parameter_names,
                                 const struct Elements *body) {
    struct Function *result = allocate(sizeof(struct Function));
    Complex_init(&result->base);
    result->base.destructor = (void (*)(struct ComplexValue *)) Function_destructor;
    result->base.type = FunctionComplexType;
    result->bindings = bindings;
    result->parameter_names = parameter_names;
    result->body = body;
    return result;
}
