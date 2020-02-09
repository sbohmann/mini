#include <stdint.h>
#include <stdbool.h>
#include <core/stringbuilder.h>
#include <minic/tokens/token_reader/token_reader.h>
#include <stdarg.h>
#include <stdlib.h>
#include "token.h"

#include "core/allocate.h"
#include "core/errors.h"
#include "generated/token_list.h"

static uint8_t tab_width = 4;

static void check_character_legality(const char *path, size_t line, size_t column, char c) {
    if (c == 0x9 || (c >= 0x20 && c < 0x7f) || (c < 0)) {
        return;
    }
    fail_with_message("Illegal character [0x%02x] at [%s], line %zu, column %zu", (uint8_t) c, path, line, column);
}

static size_t advance_column(size_t column, char c) {
    if (c == 0x9) {
        return ((column + tab_width - 1) / tab_width * tab_width);
    } else {
        return column + 1;
    }
}

static bool whitespace(char c) {
    return c == 0x9 || c == ' ';
}

static void add_token(const char *path, size_t line, size_t column, struct TokenList *tokens, struct TokenReader **current_token_reader) {
    if (*current_token_reader) {
        struct Token *token = TokenReader_result(*current_token_reader);
        TokenList_append(tokens, token);
        TokenReader_delete(*current_token_reader);
        *current_token_reader = 0;
    }
}

static void process_line(const char *path, size_t line_number, struct TokenList *tokens, const struct String *line) {
    size_t column = 1;
    struct TokenReader *current_token_reader = 0;
    size_t current_token_column = 0;
    for (size_t index = 0; index < line->length; ++index) {
        char c = line->value[index];
        check_character_legality(path, line_number, column, c);
        if (current_token_reader) {
            if (!TokenReader_add_char(current_token_reader, c)) {
                add_token(path, line_number, current_token_column, tokens, &current_token_reader);
            }
        }
        if (!whitespace(c) && !current_token_reader) {
            if (c == '#') {
                break;
            }
            if (c != ';') {
                char next = (char)(index < line->length - 1 ? line->value[index + 1] : 0);
                current_token_reader = TokenReader_create((struct Position) {path, line_number, column}, c, next);
                if (!current_token_reader) {
                    fail_with_message("Illegal character [%c] (0x%02x) at line %zu, column %zu, file [%s]",
                                      c, (int) c, line_number, column, path);
                }
                if (!TokenReader_add_char(current_token_reader, c)) {
                    fail_with_message(
                            "Newly created token reader did not acccept initial character [%c] (0x%02x) at line %zu, column %zu, file [%s]",
                            c, (int) c, line_number, column, path);
                }
            }
            current_token_column = column;
        }

        column = advance_column(column, c);
    }
    add_token(path, line_number, current_token_column, tokens, &current_token_reader);
}

static void delete_token_list(struct TokenList *tokens) {
//    struct TokenListElement *iterator = TokenList_begin(tokens);
//    while (iterator) {
//        free(TokenListIterator_get(iterator));
//    }
    TokenList_delete(tokens);
}

static struct Tokens *flatten(struct TokenList *tokens) {
    struct Tokens *result = allocate(sizeof(struct Tokens));
    result->size = TokenList_size(tokens);
    result->data = TokenList_to_array(tokens);
    delete_token_list(tokens);
    return result;
}

struct Tokens *read_tokens(const char *path, const struct Source *source) {
    struct TokenList *tokens = TokenList_create();
    for (size_t index = 0; index < source->number_of_lines; ++index) {
        const struct String *line = source->lines + index;
        process_line(path, index + 1, tokens, line);
    }
    return flatten(tokens);
}

_Noreturn void fail_at_position(struct Position position, const char *format, ...) {
    fprintf(stderr, "%s:%zu:%zu: ",
            position.path, position.line, position.column);
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fprintf(stderr, "\n");
    fail();
}

_Noreturn void fail_after_position(struct Position position, const char *format, ...) {
    fprintf(stderr, "%s:%zu:%zu: Unexpected end of input",
            position.path, position.line, position.column);
    va_list arguments;
    va_start(arguments, format);
    vfprintf(stderr, format, arguments);
    va_end(arguments);
    fprintf(stderr, "\n");
    fail();
}

void Token_delete(struct Token *instance) {
    Any_release(instance->value);
    String_delete((struct String *)instance->text);
    free(instance);
}
