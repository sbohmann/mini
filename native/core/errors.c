#include "errors.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define PRINT(format) \
    va_list arguments; \
    va_start(arguments, (format)); \
    vfprintf(stderr, (format), arguments); \
    va_end(arguments);

static void newline() {
    fputc('\n', stderr);
}

_Noreturn void fail(const char *format, ...) {
    fflush(stdout);
    if (format) {
        fputs("Failed. Reason: ", stderr);
        PRINT(format)
        newline();
    } else {
        fprintf(stderr, "fail was called.");
    }
    exit(1);
}

_Noreturn void fail_errno(const char *format, ...) {
    fflush(stdout);
    fprintf(stderr, "Failed with errno: %d - %s\n", errno, strerror(errno));
    if (format) {
        fputs("Reason: ", stderr);
        PRINT(format)
        newline();
    } else {
        fprintf(stderr, "fail was called.");
    }
    exit(1);
}

void non_null_errno(const void *pointer, const char *format, ...) {
    if (!pointer) {
        fprintf(stderr, "Null pointer detected. errno: %d - %s\n", errno, strerror(errno));
        if (format) {
            fputs("Reason: ", stderr);
            PRINT(format)
            newline();
        }
        exit(1);
    }
}

void non_negative_errno(long value, const char *format, ...) {
    if (value < 0) {
        fprintf(stderr, "Negative value detected. errno: %d - %s\n", errno, strerror(errno));
        if (format) {
            fputs("Reason: ", stderr);
            PRINT(format)
            newline();
        }
        exit(1);
    }
}
