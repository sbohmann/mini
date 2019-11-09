#include <stdio.h>

#include "tokenizer.h"

static void print_source(const struct ParsedModule *module) {
    for (size_t index = 0; index < module->source->number_of_lines; ++index) {
        printf("line %zu: [%s]\n", index, module->source->lines[index].value);
    }
}

void print_tokens(const struct ParsedModule *module) {
    for (size_t index = 0; index < module->tokens->size; ++index) {
        const struct Token *token = module->tokens->data + index;
        printf("[%s] from line %zu, column %zu, file [%s]\n",
                token->text->value, token->position.line, token->position.column, token->position.path);
    }
}

int main() {
    struct ParsedModule *module = read_file("native/minic/mini.c");
    printf("path: [%s]\n", module->path);
    printf("source: %zu lines\n", module->source->number_of_lines);
    print_source(module);
    print_tokens(module);
}
