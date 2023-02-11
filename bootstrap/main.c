#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "states/core.h"

const char *exampleReplacement(const char *name);

int main() {
    initialize(exampleReplacement);
    while (true) {
        char c = getchar();
        if (c < 0) {
            if (errno != 0) {
                fprintf(stderr, "Error while reading input: %s\n", strerror(errno));
                exit(1);
            } else {
                break;
            }
        }
        process(c);
    }
    end_of_file();
}
char buf[1024];

const char *exampleReplacement(const char *name) {
    snprintf(buf, sizeof(buf), "{{%s}}", name);
    return buf;
}
