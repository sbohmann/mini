#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

struct {
    char buffer[name_buffer_capacity];
    size_t size;
} name_state;

struct {
} end_state;

int main() {
    initialize();
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
}
