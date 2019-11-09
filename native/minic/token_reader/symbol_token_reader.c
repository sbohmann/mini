#include "symbol_token_reader.h"

#include "token_reader_struct.h"

static void process_char(char c) {

}

struct SymbolTokenReader *create_symbol_reader() {
    struct TokenReader *result = allocate(sizeof(struct TokenReader));
    TokenReader_init(process_char, 0);
    return result;
}
