#include <string.h>

static const char *start_signature = "@_";
static const size_t start_signatur_length = sizeof(signature) - 1;

static const char *end_signature = ";";
static const size_t end__signatur_length = sizeof(end) - 1;

static bool is_name_part(c) {
    return c == '_' ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9');
}
