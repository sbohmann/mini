#include "stringbuilder.h"

#include <stdlib.h>
#include <string.h>

#include "allocate.h"
#include "errors.h"

struct StringBuilder {
    size_t length;
    size_t capacity;
    char *buffer;
};

struct StringBuilder *StringBuilder_create() {
    struct StringBuilder *result = allocate(sizeof(struct StringBuilder));
    const size_t initial_capacity = 32;
    *result = (struct StringBuilder) {0, initial_capacity, allocate(initial_capacity)};
    return result;
}

struct StringBuilder *StringBuilder_delete(struct StringBuilder *instance) {
    free(instance->buffer);
    free(instance);
}

static void enlarge(struct StringBuilder *self, size_t minimum_capacity) {
    size_t new_capacity = self->capacity;
    while (new_capacity < minimum_capacity) {
        new_capacity *= 2;
        if (new_capacity == 0) {
            fail("Maximum capacity exceeded for minimum capacity %zu", minimum_capacity);
        }
    }
    char *new_buffer = allocate(new_capacity);
    memcpy(new_buffer, self->buffer, self->length);
    char *old_buffer = self->buffer;
    self->buffer = new_buffer;
    free(old_buffer);
}

struct StringBuilder *StringBuilder_append(struct StringBuilder *self, char c) {
    if (self->capacity - self->length < 1) {
        enlarge(self, self->length + 1);
    }
    self->buffer[self->length] = c;
    ++self->length;
}

struct StringBuilder *StringBuilder_append_string(struct StringBuilder *self, const struct String *value) {
    if (self->capacity - self->length < value->length) {
        enlarge(self, self->length + value->length);
    }
    memcpy(self->buffer + self->length, value->value, value->length);
    self->length += value->length;
}

struct String *StringBuilder_result(struct StringBuilder *self) {
    struct String *result = allocate(sizeof(struct String));
    char *copy = allocate_raw(self->length + 1);
    memcpy(copy, self->buffer, self->length);
    copy[self->length] = 0;
    *result = (struct String) {self->length, copy};
    return result;
}
