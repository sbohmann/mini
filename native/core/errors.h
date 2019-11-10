#pragma once

_Noreturn void fail(const char *format, ...);

_Noreturn void fail_errno(const char *format, ...);

void non_null_errno(const void *pointer, const char *format, ...);

void non_negative_errno(long value, const char *format, ...);
