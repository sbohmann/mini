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
    for (size_t index = 0; index < module->elements->size; ++index) {
        switch (module->elements->data[index].type) {
            case TokenElement:
                ParserGC_mark(module->elements->data[index].token->text);
                break;
        }
    }
    ParserGC_free();
    printf("OK.\n");

}
