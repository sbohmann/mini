#include <stdio.h>

#include "tokenizer.h"

int main() {
    struct Tokens *tokens = read_file("native/mini.c");
    printf("path: [%s]\n", tokens->path);
    printf("source: [%s]\n", tokens->source);
}
