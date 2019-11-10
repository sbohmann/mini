#pragma once

#include "minic/tokens/token.h"
#include "minic/elements/element.h"

struct ParsedModule
{
    const char *path;
    const struct Source *source;
    const struct Tokens *tokens;
    const struct Elements *elements;
};

struct ParsedModule * read_file(const char *path);
