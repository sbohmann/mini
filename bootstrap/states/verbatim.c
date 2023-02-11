#include <stdio.h>

#include "core.h"

void enter_verbatim_state() {
    state = Verbatim;
}

void verbatim_process(char c) {
    if (c == start_signature[0]) {
        enter_start_signature_state();
    } else {
        putchar(c);
    }
}
