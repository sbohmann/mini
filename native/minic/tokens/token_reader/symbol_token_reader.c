#include "string_token_reader.h"

#include "token_reader_struct.h"

#include "core/allocate.h"

static bool process_char(struct TokenReader *self, char c) {
    return is_name_part(c);
}

static struct Any create_value(struct TokenReader *self, const struct String *text) {
    struct Any result = None();
    result.type = StringLiteralType;
    result.string = text;
    return result;
}

struct TokenReader *SymbolTokenReader_create(void) {
    struct TokenReader *self = allocate(sizeof(struct TokenReader));
    TokenReader_init(SymbolToken, self, process_char, create_value, 0);
    return self;
}
