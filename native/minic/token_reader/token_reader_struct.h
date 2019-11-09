#pragma once

#include <stdbool.h>
#include "token_reader.h"

struct TokenReader {
    struct StringBuilder * buffer;
    bool (*process_char)(char);
    void (*delete)();
};

void TokenReader_init(struct TokenReader *self, bool (*process_char)(char), void (*delete)());

static bool is_numeric(char c);

bool is_lowercase(char c);

bool is_uppercase(char c);

bool is_alphabetic(char c);

static bool is_name_start(char c);

static bool is_name_part(char c);
