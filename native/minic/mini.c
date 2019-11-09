#include <stdio.h>

#include "tokenizer.h"

int main() {
    struct ParsedModule *tokens = read_file("native/minic/mini.c");
    printf("path: [%s]\n", tokens->path);
    printf("source: %zu lines\n", tokens->source->number_of_lines);
    for (size_t index = 0; index < tokens->source->number_of_lines; ++index) {
        printf("line %zu: [%s]\n", index, tokens->source->lines[index].value);
    }
}
