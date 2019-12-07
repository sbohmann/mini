#include "list.h"

#include <string.h>
#include <core/errors.h>
#include <stdlib.h>

static void List_destructor(struct List *instance) {
    for (size_t index = 0; index < instance->size; ++index) {
        Any_release(instance->data[index]);
    }
    free(instance->data);
}

struct List *List_create()
{
    struct List *result = allocate(sizeof(struct List));
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->data = allocate(32 * sizeof(struct Any));
    return result;
}

struct List *List_copy(struct List *other)
{
    struct List *result = allocate(sizeof(struct List));
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->data = allocate(other->capacity * sizeof(struct Any));
    result->capacity = other->capacity;
    for (size_t index = 0; index < other->size; ++index) {
        Any_retain(other->data[index]);
    }
    memcpy(result->data, other->data, other->size);
    result->size = other->size;
    return result;
}

struct List *List_concatenate(struct List *lhs, struct List *rhs) {
    struct List *result = allocate(sizeof(struct List));
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->capacity = lhs->capacity + rhs->capacity;
    result->data = allocate(result->capacity * sizeof(struct Any));
    for (size_t index = 0; index < lhs->size; ++index) {
        Any_retain(lhs->data[index]);
    }
    for (size_t index = 0; index < rhs->size; ++index) {
        Any_retain(rhs->data[index]);
    }
    memcpy(result->data, lhs->data, lhs->size);
    memcpy(result->data + lhs->size, rhs->data, rhs->size);
    result->size = lhs->size + rhs->size;
    return result;
}

void List_add(struct List *self, struct Any value) {
    if (self->size == self->capacity) {
        struct Any *new_data = allocate(2 * self->capacity * sizeof(struct Any));
        memcpy(new_data, self->data, self->size);
    }
    Any_retain(value);
    self->data[self->size] = value;
    ++self->size;
}

struct Any List_get(const struct List *self, size_t index) {
    if (index < self->size) {
        return self->data[index];
    } else {
        fail("Index out of bounds: %zu, size: %zu", index, self->size);
    }
}

void List_set(struct List *self, size_t index, struct Any value) {
    if (index < self->size) {
        Any_retain(value);
        self->data[index] = value;
    } else {
        fail("Index out of bounds: %zu, size: %zu", index, self->size);
    }
}
