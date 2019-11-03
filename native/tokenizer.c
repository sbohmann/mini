#include "tokenizer.h"

#include "allocate.h"

struct Tokens *read_file(const char *path) {
    struct Tokens *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    return result;
}
