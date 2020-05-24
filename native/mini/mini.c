#include <stdio.h>

#include <core/errors.h>
#include <minic/module/module.h>
#include <core/allocate.h>

int main(int argc, const char **argv) {
    if (argc != 2) {
        fail_with_message("Expecting single argument <path to source file>");
    }
    ParserGC_init();
    struct ParsedModule *module = ParsedModule_read(argv[1]);
    printf("%zu elements in parsed module\n", module->elements->size);
    ParserGC_free();
    printf("OK.\n");
}
