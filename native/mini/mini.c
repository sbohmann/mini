#include <stdio.h>

#include <core/errors.h>
#include <minic/module/module.h>
#include <core/allocate.h>

#include "ast.h"
#include "ast/read_ast.h"

int main(int argc, const char **argv) {
    if (argc != 2) {
        fail_with_message("Expecting single argument <path to source file>");
    }
    ParserGC_init();
    struct ParsedModule *module = ParsedModule_read(argv[1]);
    printf("%zu elements in parsed module\n", module->elements->size);
    ParserGC_mark(module->elements->data);
    ParserGC_mark(module->elements);
    ParserGC_mark(module);
    read_ast(module);
    ParserGC_free();
    deallocate(module->elements->data);
    deallocate(module->elements);
    deallocate(module);
    printf("OK.\n");
}
