#include <stdbool.h>

#include <core/allocate.h>

#include "token_reader_struct.h"

static bool process_char(struct TokenReader *self, char c) {
    return is_numeric(c);
}

static struct Any create_value(struct TokenReader *self, const struct String *text) {
    struct Any result = Any_create();
    result.type = Integer;
    // TODO set result.integer
    return result;
}

struct TokenReader *NumberTokenReader_create() {
    struct TokenReader *self = allocate(sizeof(struct TokenReader));
    TokenReader_init(self, process_char, create_value, 0);
    return self;
}
