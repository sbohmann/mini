#include "number_token_reader.h"

#include <stdbool.h>

#include <core/allocate.h>
#include <core/stringbuilder.h>
#include <core/errors.h>
#include <core/numbers.h>

#include "token_reader_struct.h"

struct NumberTokenReader {
    struct TokenReader base;
    bool hex;
    bool fraction;
    bool negative;
};

static bool after_single_zero(struct NumberTokenReader *self) {
    return StringBuilder_length(self->base.buffer) == 1 &&
           StringBuilder_char_at(self->base.buffer, 0) == '0';
}

static bool process_char(struct TokenReader *base, char c) {
    struct NumberTokenReader *self = (struct NumberTokenReader *) base;
    if (c == 'x' && after_single_zero(self)) {
        self->hex = true;
        return true;
    } else if (c == '-' && StringBuilder_length(self->base.buffer) == 0) {
        self->negative = true;
        return true;
    }
    return is_numeric(c);
}

static struct Any create_hex_value(struct NumberTokenReader *self, const struct String *text) {
    if (text->length < 3) {
        fail_at_position(self->base.position, "Incomplete hex literal [%s]", text->value);
    }
    struct Any result = None();
    result.type = IntegerType;
    result.integer = parse_int64_or_fail(text->value + 2, text->length - 2, 16);
    return result;
}

static struct Any create_fraction_value(struct NumberTokenReader *self, const struct String *text) {
    struct Any result = None();
    // TODO
    return result;
}

static struct Any create_decimal_value(struct NumberTokenReader *self, const struct String *text) {
    return Integer(parse_int64_or_fail(text->value, text->length, 10));
}

static struct Any create_value(struct TokenReader *base, const struct String *text) {
    struct NumberTokenReader *self = (struct NumberTokenReader *) base;
    if (self->hex && self->fraction) {
        fail_at_position(base->position, "Unsupported fractional hex literal [%s]", text->value);
    } else if (self->hex && self->negative) {
        fail_at_position(base->position, "Unsupported negative hex literal [%s]", text->value);
    } else if (self->hex) {
        return create_hex_value(self, text);
    } else if (self->fraction) {
        return create_fraction_value(self, text);
    } else {
        return create_decimal_value(self, text);
    }
}

struct TokenReader *NumberTokenReader_create() {
    struct NumberTokenReader *self = allocate(sizeof(struct NumberTokenReader));
    TokenReader_init(NumberLiteralToken, &self->base, process_char, create_value, 0);
    return &self->base;
}
