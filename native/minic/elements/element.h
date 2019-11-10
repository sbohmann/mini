#pragma once

#include <minic/tokens/token.h>

enum ElementType {
    TokenElement,
    BracketElement
};

enum BracketType {
    Paren,
    Square,
    Curly
};

struct Elements {
    size_t size;
    const struct Element *data;
};

struct BracketElement {
    enum BracketType type;
    struct Elements elements;
};

struct Element {
    enum ElementType type;
    union {
        const struct Token *token;
        struct BracketElement bracket;
    };
};

struct Elements * read_elements(struct Tokens *tokens);
