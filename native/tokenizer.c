#include "tokenizer.h"

#include "allocate.h"

void retain(struct ComplexValue *instance) {

}

void release(struct ComplexValue *instance) {

}

void append(struct StringList *list) {

}

struct Tokens *read_file(const char *path) {
    struct Tokens *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    return result;
}
