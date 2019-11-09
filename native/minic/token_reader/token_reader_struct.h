#pragma once

#include <stdbool.h>

#include "minic/types.h"

#include "token_reader.h"

struct TokenReader {
    struct Position position;
    struct StringBuilder *buffer;
    bool (*process_char)(struct TokenReader *self, char);
    struct Any (*create_value)(struct TokenReader *, const struct String *);
    void (*delete)(struct TokenReader *self);
};

void TokenReader_init(struct TokenReader *self,
                      bool (*process_char)(struct TokenReader *, char),
                      struct Any (*create_value)(struct TokenReader *, const struct String *),
                      void (*delete)(struct TokenReader *));

bool is_numeric(char c);

bool is_lowercase(char c);

bool is_uppercase(char c);

bool is_alphabetic(char c);

bool is_name_start(char c);

bool is_name_part(char c);
