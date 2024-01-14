#include <stdlib.h>

#include "core.h"
#include "name_buffer.h"

static void add_character(char c);

void enter_name_state(char c) {
    name_buffer.size = 0;
    add_character(c);
    state = Name;
}

void name_process(char c) {
    if (is_name_part(c)) {
        add_character(c);
    } else if (c == end_signature[0]) {
        enter_end_signature_state();
    } else if (c == start_signature[0]) {
        enter_start_signature_state();
    } else {
        printf("%s", start_signature);
        print_name_buffer_content(stdout);
        putchar(c);
        enter_verbatim_state();
    }
}

void name_end_of_file() {
    print_name_buffer_content(stdout);
}

static void add_character(char c) {
    if (name_buffer.size < maximum_name_length) {
        name_buffer.data[name_buffer.size] = c;
        ++name_buffer.size;
    } else {
        fprintf(stderr,
            "Exceeded maximum name length of %zu.\n"
            "Name buffer content:\n",
            maximum_name_length);
        print_name_buffer_content(stderr);
        exit(1);
    }
}
