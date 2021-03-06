#include "bracket_token_reader.h"

#include <core/stringbuilder.h>
#include <core/errors.h>

#include "token_reader_struct.h"
#include "core/allocate.h"

static bool process_char(struct TokenReader *self, char c) {
    return (StringBuilder_length(self->buffer) == 0) && is_bracket(c);
}

static struct Any create_value(struct TokenReader *self, const struct String *text) {
    struct Any result = None();
    result.type = StringLiteralType;
    result.string = text;
    return result;
}

static enum TokenType bracket_type(char c) {
    if (is_opening_bracket(c)) {
        return OpeningBracketToken;
    } else if (is_closing_bracket(c)) {
        return ClosingBracketToken;
    } else {
        fail_with_message("Not a bracket: [%c] (0x%02x)", c, (int) c);
    }
}

struct TokenReader *BracketTokenReader_create(char initial_char) {
    struct TokenReader *self = allocate(sizeof(struct TokenReader));
    TokenReader_init(bracket_type(initial_char), self, process_char, create_value, 0);
    return self;
}
