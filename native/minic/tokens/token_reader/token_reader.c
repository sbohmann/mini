#include <stdbool.h>
#include <core/stringbuilder.h>
#include <stdlib.h>
#include <minic/tokens/token.h>
#include <core/allocate.h>
#include "token_reader_struct.h"
#include "number_token_reader.h"
#include "symbol_token_reader.h"
#include "bracket_token_reader.h"
#include "operator_token_reader.h"
#include "string_token_reader.h"

struct TokenReader *TokenReader_create(struct Position position, char initial_char, char next_char) {
    struct TokenReader *result;
    
    if (is_numeric(initial_char) || (initial_char == '-' && is_numeric(next_char))) {
        result = NumberTokenReader_create();
    } else if (is_name_start(initial_char)) {
        result = SymbolTokenReader_create();
    } else if (is_bracket(initial_char)) {
        result = BracketTokenReader_create();
    } else if (is_operator_part(initial_char)) {
        result = OperatorTokenReader_create();
    } else if (is_quote(initial_char)) {
        result = StringTokenReader_create(initial_char);
    } else {
        result = 0;
    }
    
    if (result != 0) {
        result->position = position;
    }
    
    return result;
}

void TokenReader_init(enum TokenType token_type,
                      struct TokenReader *self,
                      bool (*process_char)(struct TokenReader *, char),
                      struct Any (*create_value)(struct TokenReader *, const struct String *),
                      void (*delete)()) {
    self->buffer = StringBuilder_create();
    self->token_type = token_type;
    self->process_char = process_char;
    self->create_value = create_value;
    self->delete = delete;
}

void TokenReader_delete(struct TokenReader *instance) {
    if (instance->delete) {
        instance->delete(instance);
    }
    StringBuilder_delete(instance->buffer);
    free(instance);
}

bool TokenReader_add_char(struct TokenReader *self, char c) {
    bool result = self->process_char(self, c);
    if (result) {
        StringBuilder_append(self->buffer, c);
    }
    return result;
}

struct Token *TokenReader_result(struct TokenReader *self) {
    struct Token *result = allocate(sizeof(struct Token));
    result->type = self->token_type;
    result->text = StringBuilder_result(self->buffer);
    result->value = self->create_value(self, result->text);
    result->position = self->position;
    return result;
}

bool is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

bool is_lowercase(char c) {
    return c >= 'a' && c <= 'z';
}

bool is_uppercase(char c) {
    return c >= 'A' && c <= 'Z';
}

bool is_alphabetic(char c) {
    return is_lowercase(c) || is_uppercase(c);
}

bool is_name_start(char c) {
    return is_numeric(c) || is_alphabetic(c) || (c == '_');
}

bool is_name_part(char c) {
    return is_numeric(c) || is_name_start(c);
}

bool is_opening_bracket(char c) {
    switch (c) {
        case '(':
        case '[':
        case '{':
            return true;
        default:
            return false;
    }
}

bool is_closing_bracket(char c) {
    switch (c) {
        case ')':
        case ']':
        case '}':
            return true;
        default:
            return false;
    }
}

bool is_bracket(char c) {
    return is_opening_bracket(c) || is_closing_bracket(c);
}

bool is_operator_part(char c) {
    switch (c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '^':
        case '=':
        case '!':
        case '~':
        case '<':
        case '>':
        case '%':
        case '.':
        case ',':
            return true;
        default:
            return false;
    }
}

bool is_quote(char c) {
    return c == '"' || c == '\'';
}
