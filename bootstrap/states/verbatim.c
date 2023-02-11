#include <stdio.h>

#include "core.h"

void enter_verbatim_state() {
    state = Verbatim;
    if (debug) printf("Entered state Verbatim\n");
}

void verbatim_process(char c) {
    if (c == start_signature[0]) {
        enter_start_signature_state();
    } else {
        putchar(c);
    }
}
