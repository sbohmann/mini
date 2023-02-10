#include <string.h>

static const char *signature = "@_";
static const size_t signature_length = sizeof(signature) - 1;

static const char *end = ";";
static const size_t end_length = sizeof(end) - 1;

static bool is_name_part(c) {
	return c == '_' ||
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9');
}
