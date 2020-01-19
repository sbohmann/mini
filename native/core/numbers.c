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

static struct IntegerParsingResult error(const char *message) {
    struct IntegerParsingResult result;
    result.success = false;
    result.index = 0;
    result.index_relevant = false;
    return result;
}

static struct IntegerParsingResult error_with_index(const char *message, size_t index) {
    struct IntegerParsingResult result;
    result.success = false;
    result.index = index;
    result.index_relevant = true;
    return result;
}

struct IntegerParsingResult parse_int64(const char *str, size_t len, uint8_t radix) {
    int64_t result = 0;
    int64_t limit = 0;
    int64_t limitBeforeShift = 0;
    
    bool positive = true;
    bool firstDigit = true;
    
    for (size_t idx = 0; idx < len; ++idx) {
        char c = str[idx];
        if (c == 0) {
            return error_with_index("Hit a null character", idx);
        }
        
        bool skip = false;
        
        if (idx == 0) {
            if (c == '-') {
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
                error("Literal out of range");
            }
            
            result = (int64_t) (result * radix);
            
            int8_t digit = parse_digit(c);
            
            if (digit < 0 || digit >= radix) {
                error_with_index("Illegal character", idx + 1);
            }
            
            if ((positive && (int64_t) (digit) > (limit - result)) ||
                (positive == false && (int64_t) (-digit) < (limit - result))) {
                error("Literal out of range");
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
        error("No digits found");
    }
    
    struct IntegerParsingResult integerParsingResult;
    integerParsingResult.success = true;
    integerParsingResult.signed_result = result;
    return integerParsingResult;
}

struct IntegerParsingResult parse_uint64(const char *str, size_t len, uint8_t radix) {
    uint64_t result = 0;
    uint64_t limit = 0;
    uint64_t limitBeforeShift = 0;
    
    bool firstDigit = true;
    
    for (size_t idx = 0; idx < len; ++idx) {
        char c = str[idx];
        if (c == 0) {
            error_with_index("Hit a null character", idx);
        }
    
        limit = UINT64_MAX;
        limitBeforeShift = (int64_t) (limit / radix);
        
        if (result > limitBeforeShift) {
            error("Literal out of range");
        }
        
        result = (int64_t) (result * radix);
        
        int8_t digit = parse_digit(c);
        
        if (digit < 0 || digit >= radix) {
            error_with_index("Illegal character", idx + 1);
        }
        
        if ((int64_t) (digit) > (limit - result)) {
            error("Literal out of range");
        }
        
        result = (int64_t) (result + digit);
        
        firstDigit = false;
    }
    
    if (firstDigit) {
        error("No digits found");
    }
    
    struct IntegerParsingResult integerParsingResult;
    integerParsingResult.success = true;
    integerParsingResult.unsigned_result = result;
    return integerParsingResult;
}

static _Noreturn void parsing_failed(struct IntegerParsingResult result, const char *str, size_t len, uint8_t radix) {
    if (result.index_relevant) {
        fail_with_message("%s - index %zu, len %zu, literal [%s] while parsing an integer with radix %d",
                          result.error_message, result.index, len, str, radix);
    } else {
        fail_with_message("%s - len %zu, literal [%s] while parsing an integer with radix %d",
                          result.error_message, len, str, radix);
    }
}

int64_t parse_int64_or_fail(const char *str, size_t len, uint8_t radix) {
    struct IntegerParsingResult result = parse_int64(str, len, radix);
    if (result.success) {
        return result.signed_result;
    } else {
        parsing_failed(result, str, len, radix);
    }
}

uint64_t parse_uint64_or_fail(const char *str, size_t len, uint8_t radix) {
    struct IntegerParsingResult result = parse_uint64(str, len, radix);
    if (result.success) {
        return result.unsigned_result;
    } else {
        parsing_failed(result, str, len, radix);
    }
}
