#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "configuration.h"

enum State {
    Verbatim,
    StartSignature,
    Name,
    EndSignature
};

enum State state;

struct {
    size_t offset;
} signature_state;

struct {
    char buffer[name_buffer_capacity];
    size_t size;
} name_state;

struct {
} end_state;

static void init() {
    state = Verbatim;
}

static void process(char c) {
    switch (state) {
        case Verbatim:
            if (c == start_signature[0]) {
                signature_state.offset = 1;
                state = StartSignature;
            } else {
                putchar(c);
            }
            break;
        case StartSignature:
            if (signature_state.offset < start_signature_length - 1) {
                if (c == start_signature[signature_state.offset]) {
                    ++signature_state.offset;
                } else {
                    // TODO write signature to offset as verbatim
                    // and set state to Verbatim
                }
            } else {
                name_state.buffer[0] = c;
                name_state.size = 1;
                state = Name;
            }
    }
}

int main() {
    init();
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
