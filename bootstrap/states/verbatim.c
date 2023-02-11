#include "core.h"

void verbatim_process(char c) {
    if (c == start_signature[0]) {
        enter_start_signature_state();
    } else {
        putchar(c);
    }
}
