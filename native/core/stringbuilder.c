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

struct StringBuilder *StringBuilder_create(void) {
    struct StringBuilder *result = allocate(sizeof(struct StringBuilder));
    const size_t initial_capacity = 32;
    *result = (struct StringBuilder) {0, initial_capacity, allocate(initial_capacity)};
    return result;
}

void StringBuilder_delete(struct StringBuilder *instance) {
    deallocate(instance->buffer);
    deallocate(instance);
}

size_t StringBuilder_length(struct StringBuilder *self) {
    return self->length;
}

char StringBuilder_char_at(struct StringBuilder *self, size_t index) {
    if (index < self->length) {
        return self->buffer[index];
    } else {
        fail_with_message("StringBuilder_char_at: index %zu out of bounds. Length: %zu", index, self->length);
    }
}

static void enlarge(struct StringBuilder *self, size_t minimum_capacity) {
    size_t new_capacity = self->capacity;
    while (new_capacity < minimum_capacity) {
        new_capacity *= 2;
        if (new_capacity == 0) {
            fail_with_message("Maximum capacity exceeded for minimum capacity %zu", minimum_capacity);
        }
    }
    char *new_buffer = allocate(new_capacity);
    memcpy(new_buffer, self->buffer, self->length);
    char *old_buffer = self->buffer;
    self->buffer = new_buffer;
    deallocate(old_buffer);
}

void StringBuilder_append(struct StringBuilder *self, char c) {
    if (self->capacity - self->length < 1) {
        enlarge(self, self->length + 1);
    }
    self->buffer[self->length] = c;
    ++self->length;
}

void StringBuilder_append_string(struct StringBuilder *self, const struct String *value) {
    if (self->capacity - self->length < value->length) {
        enlarge(self, self->length + value->length);
    }
    memcpy(self->buffer + self->length, value->value, value->length);
    self->length += value->length;
}

const struct String *StringBuilder_result(struct StringBuilder *self) {
    return String_from_buffer(self->buffer, self->length);
}
