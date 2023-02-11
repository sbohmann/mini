#include <stddef.h>

#include "configuration.h"

#define START_SIGNATURE "@_"
#define END_SIGNATURE ";"

const char *start_signature = START_SIGNATURE;
const size_t start_signature_length = sizeof(START_SIGNATURE) - 1;

const char *end_signature = END_SIGNATURE;
const size_t end_signature_length = sizeof(END_SIGNATURE) - 1;

bool is_name_part(char c) {
    return c == '_' ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9');
}
