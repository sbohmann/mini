#include <stdbool.h>
#include <core/stringbuilder.h>
#include <stdlib.h>
#include "token_reader_struct.h"
#include "number_token_reader.h"

struct TokenReader *TokenReader_create(char initial_char) {
    struct TokenReader *result;
    if (is_numeric(initial_char)) {
        result = create_number_reader();
    } else if (is_name_start(initial_char)) {
        result = create_symbol_reader();
    } else {
        result = 0;
    }
    return result;
}

void TokenReader_init(struct TokenReader *self, bool (*process_char)(char), void (*delete)()) {
    self->buffer = StringBuilder_create();
    self->process_char= process_char;
    self->delete = delete;
}

void TokenReader_delete(struct TokenReader *instance) {
    if (instance->delete) {
        instance->delete();
    }
    StringBuilder_delete(instance->buffer);
    free(instance);
}

bool TokenReader_add_char(struct TokenReader *self, char c) {
    bool result = self->process_char(c);
    if (result) {
        StringBuilder_append(self->buffer, c);
    }
    return result;
}

struct Token *TokenReader_result() {

}

static bool is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

bool is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

bool is_uppercase(char c) {
    return c >= 'A' && c <= 'Z';
}

bool is_alphabetic(char c) {
    return is_lowercase(c) || is_uppercase(c);
}

static bool is_name_start(char c) {
    return is_numeric(c) || is_alphabetic(c) || (c == '_');
}

static bool is_name_part(char c) {
    return is_numeric(c) || is_name_part(c);
}
