#include "allocate.h"

#include <stdlib.h>
#include <string.h>

#include "errors.h"

void * allocate(size_t size) {
    void *result = allocate_raw(size);
    memset(result, 0, size);
    return result;
}

void * allocate_raw(size_t size) {
    void *result = malloc(size);
    if (!result) {
        fail("Failed to allocate %d bytes of memory.", size);
    }
    return result;
}
