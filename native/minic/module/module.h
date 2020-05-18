#pragma once

#include "minic/source.h"
#include "minic/tokens/token.h"
#include "minic/elements/element.h"

struct ParsedModule
{
    const char *path;
    const struct Source *source;
    const struct Tokens *tokens;
    const struct Elements *elements;
    //const struct
};

struct ParsedModule * ParsedModule_read(const char *path);

void ParsedModule_delete(struct ParsedModule *instance);
