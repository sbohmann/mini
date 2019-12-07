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

struct List *List_create() {
    struct List *result = allocate(sizeof(struct List));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->base.type = ListComplexType;
    const size_t start_capacity = 32;
    result->data = allocate(start_capacity * sizeof(struct Any));
    result->capacity = start_capacity;
    return result;
}

struct List *List_copy(const struct List *other) {
    struct List *result = allocate(sizeof(struct List));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->base.type = ListComplexType;
    result->data = allocate(other->capacity * sizeof(struct Any));
    result->capacity = other->capacity;
    for (size_t index = 0; index < other->size; ++index) {
        Any_retain(other->data[index]);
    }
    memcpy(result->data, other->data, other->size * sizeof(struct Any));
    result->size = other->size;
    return result;
}

struct List *List_concatenate(const struct List *lhs, const struct List *rhs) {
    struct List *result = allocate(sizeof(struct List));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *))List_destructor;
    result->base.type = ListComplexType;
    result->capacity = (lhs->capacity > rhs->capacity ? lhs->capacity : rhs->capacity);
    if (result->capacity < lhs->size + rhs->size) {
        result->capacity *= 2;
    }
    if (result->capacity < lhs->size + rhs->size) {
        fail("Logical error");
    }
    result->data = allocate(result->capacity * sizeof(struct Any));
    for (size_t index = 0; index < lhs->size; ++index) {
        Any_retain(lhs->data[index]);
    }
    for (size_t index = 0; index < rhs->size; ++index) {
        Any_retain(rhs->data[index]);
    }
    memcpy(result->data, lhs->data, lhs->size * sizeof(struct Any));
    memcpy(result->data + lhs->size, rhs->data, rhs->size * sizeof(struct Any));
    result->size = lhs->size + rhs->size;
    return result;
}

void List_add(struct List *self, struct Any value) {
    if (self->size == self->capacity) {
        size_t new_capacity = 2 * self->capacity;
        if (new_capacity <= self->capacity) {
            fail("Exceeded maximum list capacity");
        }
        struct Any *new_data = allocate(new_capacity * sizeof(struct Any));
        self->capacity = new_capacity;
        memcpy(new_data, self->data, self->size * sizeof(struct Any));
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
