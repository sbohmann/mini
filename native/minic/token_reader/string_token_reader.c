#include <core/stringbuilder.h>
#include <core/errors.h>
#include "token_reader_struct.h"

#include "core/allocate.h"

struct StringTokenReader {
    struct TokenReader base;
    char quote_char;
    bool escaped;
    bool closed;
    struct StringBuilder *value;
};

static char escape_sequence(char c) {
    switch(c) {
        case 'r': return '\r';
        case 'n': return '\n';
        case 't': return '\t';
        default:
            return c;
    }
}

static bool process_char(struct TokenReader *base, char c) {
    struct StringTokenReader *self = (struct StringTokenReader *) base;
    if (self->closed) {
        return false;
    } else if (StringBuilder_length(base->buffer) == 0) {
        if (c != self->quote_char) {
            fail("Logical error: first character [%c] (0x%02x) != quote char [%c] (0x%02x)",
                    c, (int)c, self->quote_char, (int)self->quote_char);
        }
    } else if (self->escaped) {
        StringBuilder_append(self->value, escape_sequence(c));
    } else {
        if (c == self->quote_char) {
            self->closed = true;
        } else {
            if (c == '\\') {
                self->escaped = true;
            } else {
                StringBuilder_append(self->value, c);
            }
        }
    }
    return true;
}

static struct Any create_value(struct TokenReader *self, const struct String *text) {
    struct Any result = Any_create();
    result.type = String;
    result.string = text;
    return result;
}

struct TokenReader *StringTokenReader_create(char quote_char) {
    struct StringTokenReader *self = allocate(sizeof(struct StringTokenReader));
    TokenReader_init(&self->base, process_char, create_value, 0);
    self->quote_char = quote_char;
    return &self->base;
}
