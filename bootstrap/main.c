#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "replacementfile/replacementfile.h"
#include "states/states.h"

static void read_replacements();

struct ReplacementFile *replacement_file;

static const char *replace_from_file(const char *name);

int main() {
    read_replacements();
    states_initialize(replace_from_file);
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

static void read_replacements() {
    const char *path = "replacements";
    FILE * input_file = fopen(path, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Failed to open path [%s]\n", path); 
        exit(1);
    }
    replacement_file = replacementfile_new(input_file);
}

static const char *replace_from_file(const char *name) {
    return replacementfile_replace(replacement_file, name);
}
