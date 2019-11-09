#include <stdint.h>
#include "tokenizer.h"

#include "core/allocate.h"
#include "core/errors.h"
#include "generated/token_list.h"

static uint8_t tab_width = 4;

static struct Tokens *flatten(struct TokenList *tokens) {
    const size_t size = TokenList_size(tokens);
    struct Tokens *result = allocate(sizeof(struct Tokens) * size);
    result->size = size;
    TokenList_delete(tokens);
    return result;
}

static void check_character_legality(const char *path, size_t line, size_t column, char c) {
    if (c == 0x9 || (c >= 0x20 && c < 0x7f)) {
        return;
    }
    fail("Illegal character [%02x] at [%s], line %zu, column %zu", (int)c, path, line, column);
}

static size_t advance_column(size_t column, char c) {
    if (c == 0x9) {
        return ((column + tab_width - 1) / tab_width * tab_width) + 0;
    } else {
        return column + 1;
    }
}

static void process_line(const char *path, size_t line_number, struct TokenList *tokens, const struct String *line) {
    size_t column = 1;
    for (size_t index = 0; index < line->length; ++index) {
        char c = line->value[index];
        check_character_legality(path, line_number, column, c);
        
        column = advance_column(column, c);
    }
}

static struct Tokens *read_tokens(const char *path, const struct Source *source) {
    struct TokenList *tokens = TokenList_create();
    for (size_t index = 0; index < source->number_of_lines; ++index) {
        const struct String *line = source->lines + index;
        process_line(path, index + 1, tokens, line);
    }
    return flatten(tokens);
}

struct ParsedModule *read_file(const char *path) {
    struct ParsedModule *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    result->tokens = read_tokens(path, result->source);
    return result;
}
