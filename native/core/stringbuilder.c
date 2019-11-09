#include "stringbuilder.h"

#include <stdlib.h>
#include <string.h>

#include "allocate.h"

struct StringBuilder {
    size_t length;
    size_t capacity;
    char *buffer;
};

struct StringBuilder *StringBuilder_create() {
    struct StringBuilder *result = allocate(sizeof(struct StringBuilder));
    const size_t initial_size = 32;
    *result = (struct StringBuilder) {0, initial_size, allocate(initial_size)};
}

struct StringBuilder *StringBuilder_delete(struct StringBuilder *instance) {
    free(instance->buffer);
    free(instance);
}

static void enlarge(struct StringBuilder *self, size_t minimum_length) {
    size_t new_length = self->length * 2;
    while (new_length < minimum_length) {
        new_length *= 2;
        
    }
    char *new_buffer = allocate(new_length);
    
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
}

struct String *StringBuilder_result(struct StringBuilder *self) {

}
