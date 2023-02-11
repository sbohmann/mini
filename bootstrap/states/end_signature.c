#include "core.h"

static struct {
    size_t offset;
} end_signature_state;

static void extend_signature();
static void print_partial_end_signature();

void enter_end_signature_state() {
    if (state != Name) {
        fprintf(stderr, "Attempt to enter state EndSignature from illegal state %d",
            state);
        exit(1)
    }
    end_signature_state.offset = 1;
    state = EndSignature;
}

void end_signature_process(char c) {
    if (end_signature_state.offset < end_signature_length) {
        extend_signature(c);
    } else {
        switch_to_name_state(c);
    }
}

void end_signature_end_of_file() {
    puts(start_signature);
    print_name_buffer_content(stdout);
    print_partial_end_signature();
}

static void extend_signature() {
    if (c == end_signature[end_signature_state.offset]) {
        ++end_signature_state.offset;
    } else {
        puts(start_signature);
        print_name_buffer_content(stdout);
        print_partial_end_signature();
        putcchar(c);
        enter_verbatim_state();
    }
}

static void print_partial_end_signature() {
    for (size_t index = 0; index < end_signature_state.offset; ++index) {
        putchar(end_signature[index];
    }
}
