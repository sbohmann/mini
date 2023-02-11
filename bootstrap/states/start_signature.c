#include <stdio.h>

#include "core.h"

static struct {
    size_t offset;
} start_signature_state;

static void print_partial_start_signature();
static void extend_signature(char c);
static void switch_to_name_state(char c);

void enter_start_signature_state() {
    start_signature_state.offset = 1;
    state = StartSignature;
}

void start_signature_process(char c) {
    if (start_signature_state.offset < start_signature_length) {
        extend_signature(c);
    } else {
        switch_to_name_state(c);
    }
}

void start_signature_end_of_file() {
    print_partial_start_signature();
}

static void extend_signature(char c) {
    if (c == start_signature[start_signature_state.offset]) {
        ++start_signature_state.offset;
    } else {
        print_partial_start_signature();
        putchar(c);
        enter_verbatim_state();
    }
}

static void switch_to_name_state(char c) {
    if (is_name_part(c)) {
        enter_name_state(c);
    } else {
        printf("%s", start_signature);
        enter_verbatim_state();
    }
}

static void print_partial_start_signature() {
    for (size_t index = 0; index < start_signature_state.offset; ++index) {
        putchar(start_signature[index]);
    }
}
