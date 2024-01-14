#include <stdlib.h>
#include <memory.h>

#include "symbol.h"

struct Symbol * Symbol_create(const char *name) {
    struct Symbol *result = malloc(sizeof(struct Symbol));
    memcpy(result,
           &(struct Symbol) {
                   name,
                   sizeof(name)
           },
           sizeof(struct Symbol));
    return result;
}
