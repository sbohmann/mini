#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "name_buffer.h"

static struct {
    size_t offset;
} end_signature_state;

static void extend_signature(char c);
static void print_partial_end_signature();
static void print_result();

void enter_end_signature_state() {
    if (state != Name) {
        fprintf(stderr, "Attempt to enter state EndSignature from illegal state %d",
            state);
        exit(1);
    }
    end_signature_state.offset = 1;
    state = EndSignature;
}

void end_signature_process(char c) {
    if (end_signature_state.offset < end_signature_length) {
        extend_signature(c);
    } else {
        print_result();
        putchar(c);
        enter_verbatim_state();
    }
}

void end_signature_end_of_file() {
    if (end_signature_state.offset < end_signature_length) {
        puts(start_signature);
        print_name_buffer_content(stdout);
        print_partial_end_signature();
    } else {
        print_result();
    }
}

static void extend_signature(char c) {
    if (c == end_signature[end_signature_state.offset]) {
        ++end_signature_state.offset;
    } else {
        puts(start_signature);
        print_name_buffer_content(stdout);
        print_partial_end_signature();
        putchar(c);
        enter_verbatim_state();
    }
}

static void print_partial_end_signature() {
    for (size_t index = 0; index < end_signature_state.offset; ++index) {
        putchar(end_signature[index]);
    }
}

static void print_result() {
    putchar('{');
    putchar('{');
    print_name_buffer_content(stdout);
    putchar('}');
    putchar('}');
}
