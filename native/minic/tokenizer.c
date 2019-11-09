#include <stdint.h>
#include <stdbool.h>
#include <core/stringbuilder.h>
#include "tokenizer.h"

#include "core/allocate.h"
#include "core/errors.h"
#include "generated/token_list.h"

static uint8_t tab_width = 4;

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

static bool whitespace(char c) {
    return c == 0x9 || c == ' ';
}

static void add_token(const char *path, size_t line, size_t column, struct TokenList *tokens, struct StringBuilder **current_token_text) {
    if (*current_token_text) {
        struct Token *token = allocate(sizeof(struct Token));
        struct String *text = StringBuilder_result(*current_token_text);
        token->text = text;
        token->position = (struct Position) {path, line, column};
        TokenList_append(tokens, token);
        StringBuilder_delete(*current_token_text);
        *current_token_text = 0;
    }
}

static void process_line(const char *path, size_t line_number, struct TokenList *tokens, const struct String *line) {
    size_t column = 1;
    struct TokenReader *current_token_reader = 0;
    size_t current_token_column = 0;
    bool inside_string_literal = 0;
//    for (size_t index = 0; index < line->length; ++index) {
//        char c = line->value[index];
//        check_character_legality(path, line_number, column, c);
//        if (whitespace(c)) {
//            add_token(path, line_number, current_token_column, tokens, &current_token_text);
//        } else {
//            if (!current_token_text) {
//                current_token_reader = TokenReader_create();
//                if (!current_token_reader) {
//                    fail("Illegal character [%c] (%02x) at line %zu, column %zu, file [%s]",
//                            c, (int)c, line_number, column, path);
//                }
//                current_token_column = column;
//            }
//            StringBuilder_append(current_token_text, c);
//        }
//        column = advance_column(column, c);
//    }
//    add_token(path, line_number, current_token_column, tokens, &current_token_text);
}


static struct Tokens *flatten(struct TokenList *tokens) {
    const size_t size = TokenList_size(tokens);
    struct Tokens *result = allocate(sizeof(struct Tokens) * size);
    result->size = size;
    result->data = TokenList_to_array(tokens);
    TokenList_delete(tokens);
    return result;
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
