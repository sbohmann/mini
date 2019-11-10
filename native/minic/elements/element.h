#pragma once

#include <minic/tokens/tokenizer.h>

enum ElementType {
    Token,
    Bracket
};

enum BracketType {
    Paren,
    Square,
    Curly
};

struct BracketElement {
    enum BracketType type;
    size_t size;
    struct Element *elements;
};

struct Element {
    enum ElementType type;
    union {
        struct Token *token;
        struct BracketElement bracket;
    };
};
