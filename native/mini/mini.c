#include <stdio.h>

#include <core/errors.h>
#include <minic/module/module.h>

int main(int argc, const char **argv) {
    if (argc != 2) {
        fail_with_message("Expecting single argument <path to source file>");
    }
    struct ParsedModule *module = ParsedModule_read(argv[1]);
    printf("OK.")
}
