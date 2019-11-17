#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <core/errors.h>
#include <core/numbers.h>

#include "collections/hashmap.h"
#include "minic/any.h"

bool duplication() {
    size_t value = 1;
    while (1) {
        value *= 2;
        printf("%zu\n", value);
        if (value == 0) {
            return 1;
        }
    }
}

void print(struct Any value) {
    switch (value.type) {
        case NoneType:
            printf("None");
            break;
        case IntegerType:
            printf("%d", (int) value.integer);
            break;
        case StringType:
            printf("%s", value.string->value);
            break;
        case ComplexType:
            // TODO
            printf("<complex>");
            break;
        case FlatType:
            printf("[");
            for (size_t index = 0; index < 8; ++index) {
                printf("%s%02x", (index > 0 ? " " : ""), (uint8_t) value.flat_value[index]);
            }
            printf("]");
            break;
        default:
            printf("<unknown>");
    }
    putchar('\n');
}

size_t determine_high_start() {
    if (sizeof(size_t) == 8) {
        return (size_t) 18446744073707551614u;
    } else {
        return (size_t) 4292967294u;
    }
}

#define PRN if (key % 1000 == 0) { printf("%zu\n", key); }

void hashmap() {
    struct HashMap *map = HashMap_create();
    size_t key = 1234567;
    print(HashMap_get(map, Integer(key)));
    HashMap_put(map, Integer(key), String("Hi! :D"));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_put(map, Integer(key + 1), String("Ok ^^"));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_remove(map, Integer(key));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_remove(map, Integer(key + 1));
    
    for (key = 0; key < 2000000; key += 3) {
        PRN
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%zu", key);
        struct Any value;
        value.type = StringType;
        value.string = String_from_buffer(buffer, strlen(buffer));
        HashMap_put(map, Integer(key), value);
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            size_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, Integer(key));
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN
        if (key % 5 == 0) {
            HashMap_remove(map, Integer(key));
        }
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0 && key % 5 != 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            size_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, Integer(key));
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    size_t high_start = determine_high_start();
    
    for (key = high_start; key >= high_start ; key += 3) {
        PRN
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%zu", key);
        struct Any value;
        value.type = StringType;
        value.string = String_from_buffer(buffer, strlen(buffer));
        HashMap_put(map, Integer(key), value);
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            size_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, Integer(key));
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN
        if (key % 5 == 0) {
            HashMap_remove(map, Integer(key));
        }
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0 && key % 5 != 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            size_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, Integer(key));
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
}

#define TEST(name) if (!name()) { fprintf(stderr, "Test case " #name " failed.\n"); exit(1); }

int main() {
    hashmap();
    printf("Success.\n");
}
