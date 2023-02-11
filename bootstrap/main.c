#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "replacementfile/replacementfile.h"
#include "states/states.h"

const char *exampleReplacement(const char *name);

static void read_replacements();

struct FileReplacements *file_replacements;

int main() {
    read_replacements();
    states_initialize(exampleReplacement);
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

static void read_replacements() {
    const char *path = "replacements";
    FILE * input_file = fopen(path, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open path [%s]\n", path); 
        abort();
    }
    replacementfile_new(input_file);
//     file_replacements = 
}
