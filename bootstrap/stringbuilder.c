#include <stddef.h>
#include <stdio.h>

static char buffer[1024] = {0};
static const size_t max_length = sizeof(buffer) - 1;
static size_t length = 0;

void stringbuilder_append(char c) {
    if (length >= max_length) {
        fprintf(stderr, "Maximum name length of %zu exceeded.\nName: %s\n",
            max_length, buffer);
    }
    buffer[length] = c;
    buffer[length + 1] = 0;
    ++length;
}

const char * stringbuilder_value() {
    return buffer;
}

void stringbuilder_reset() {
    buffer[0] = 0;
    length = 0;
}
