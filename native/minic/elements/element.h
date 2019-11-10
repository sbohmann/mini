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
    const struct Token *opening_bracket;
    const struct Token *closing_bracket;
};

struct Element {
    struct Position position;
    enum ElementType type;
    union {
        const struct Token *token;
        struct BracketElement bracket;
    };
};

struct Elements * read_elements(const struct Tokens *tokens);
