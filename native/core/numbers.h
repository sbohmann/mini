#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct String;

struct IntegerParsingResult {
    bool success;
    union {
        int64_t signed_result;
        uint64_t unsigned_result;
        struct {
            const struct String *error_message;
            size_t index;
            bool index_relevant;
        };
    };
};

struct IntegerParsingResult parse_int64(const char *str, size_t len, uint8_t radix);
struct IntegerParsingResult parse_uint64(const char *str, size_t len, uint8_t radix);

int64_t parse_int64_or_fail(const char *str, size_t len, uint8_t radix);
uint64_t parse_uint64_or_fail(const char *str, size_t len, uint8_t radix);
