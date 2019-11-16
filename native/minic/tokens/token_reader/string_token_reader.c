#include "string_token_reader.h"

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
        self->escaped = false;
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

static struct Any create_value(struct TokenReader *base, const struct String *text) {
    struct StringTokenReader *self = (struct StringTokenReader *) base;
    if (!self->closed) {
        fail_at_position(base->position, "Unclosed string literal");
    }
    struct Any result = None();
    result.type = StringType;
    result.string = StringBuilder_result(self->value);
    return result;
}

void delete(struct TokenReader *base) {
    struct StringTokenReader *self = (struct StringTokenReader *) base;
    StringBuilder_delete(self->value);
}

struct TokenReader *StringTokenReader_create(char quote_char) {
    struct StringTokenReader *self = allocate(sizeof(struct StringTokenReader));
    TokenReader_init(StringLiteral, &self->base, process_char, create_value, delete);
    self->quote_char = quote_char;
    self->value = StringBuilder_create();
    return &self->base;
}
