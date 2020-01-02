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

const char *bracket_type_name(enum BracketType type);

struct Elements {
    size_t size;
    const struct Element *data;
};

struct ElementList;

struct Elements *Elements_from_list(struct ElementList *elements);

struct BracketElement {
    enum BracketType type;
    const struct Elements *elements;
    const struct Token *opening_bracket;
    const struct Token *closing_bracket;
};

struct Element {
    struct Position position;
    enum ElementType type;
    union {
        const struct Token *token;
        struct BracketElement *bracket;
    };
};

struct Elements * read_elements(const struct Tokens *tokens);
