#include "numbers.h"

#include "errors.h"

static int8_t parse_digit(char c) {
    if (c >= '0' && c <= '9') {
        return (int8_t)(c - '0');
    } else if (c >= 'A' && c <= 'Z') {
        return (int8_t)(c - 'A' + 10);
    } else if (c >= 'a' && c <= 'z') {
        return (int8_t)(c - 'a' + 10);
    } else {
        return -1;
    }
}

int64_t parse_integer(const char *str, size_t len, uint8_t radix, bool isSigned) {
    int64_t result = 0;
    int64_t limit = 0;
    int64_t limitBeforeShift = 0;
    
    bool positive = true;
    bool firstDigit = true;
    
    for (size_t idx = 0; idx < len; ++idx) {
        char c = str[idx];
        if (c == 0) {
            fail("Hit a null character at index 5zu with len %zu in literal [%s] while parsing an integer with radix %d",
                 idx, len, str, (int) radix);
        }
        
        bool skip = false;
        
        if (idx == 0) {
            if (isSigned && c == '-') {
                positive = false;
                skip = true;
            }
            
            if (positive) {
                limit = INT64_MAX;
                limitBeforeShift = (int64_t) (limit / radix);
            } else {
                limit = INT64_MIN;
                limitBeforeShift = (int64_t) (limit / radix);
            }
        }
        
        if (skip == false) {
            if ((positive && result > limitBeforeShift) || (positive == false && result < limitBeforeShift)) {
                fail("Literal out of range: [%s] while parsing an integer with radix %d",
                     str, (int) radix);
            }
            
            result = (int64_t) (result * radix);
            
            int8_t digit = parse_digit(c);
            
            if (digit < 0 || digit >= radix) {
                fail("Illegal character [%c] (0x%02x)] at position %zu in string [%s] while parsing an integer with radix %d",
                     c, (int) c, idx + 1, str, (int) radix);
            }
            
            if ((positive && (int64_t)(digit) > (limit - result)) ||
                (positive == false && (int64_t)(-digit) < (limit - result))) {
                fail("Literal out of range: [%s] while parsing an integer with radix %d",
                     str, (int) radix);
            }
            
            if (positive) {
                result = (int64_t) (result + digit);
            } else {
                result = (int64_t) (result - digit);
            }
            
            firstDigit = false;
        }
    }
    
    if (firstDigit) {
        fail("No digits found in string %s while parsing an integer with radix %d",
             str, (int) radix);
    }
    
    return result;
}
