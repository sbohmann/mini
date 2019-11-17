#include <stdio.h>
#include <string.h>

#include <core/errors.h>
#include <core/numbers.h>
#include <inttypes.h>

#include "collections/hashmap.h"
#include "minic/any.h"

static void print(struct Any value) {
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

#define PRN(message) if (key % 1000 == 0) { printf(#message ": %" PRIu64 "\n", key); }

static void hash_map() {
    struct HashMap *map = HashMap_create();
    uint64_t key = 1234567;
    print(HashMap_get(map, Integer(key)));
    HashMap_put(map, Integer(key), String(String_from_literal("Hi! :D")));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_put(map, Integer(key + 1), String(String_from_literal("Ok ^^")));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_remove(map, Integer(key));
    print(HashMap_get(map, Integer(key)));
    print(HashMap_get(map, Integer(key + 1)));
    HashMap_remove(map, Integer(key + 1));
    
    for (key = 0; key < 2000000; key += 3) {
        PRN(build)
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, key);
        struct Any value;
        value.type = StringType;
        value.string = String_from_buffer(buffer, strlen(buffer));
        HashMap_put(map, Integer(key), value);
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN(check)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            size_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN(prune)
        if (key % 5 == 0) {
            HashMap_remove(map, Integer(key));
        }
    }
    
    for (key = 0; key < 2000000; ++key) {
        PRN(check pruned)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0 && key % 5 != 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            uint64_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%zu] != key [%zu]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    uint64_t middle_start = 4293967293u;
    uint64_t middle_end = 4295967294u;
    
    for (key = middle_start; key < middle_end; key += 3) {
        PRN(build middle)
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, key);
        struct Any value;
        value.type = StringType;
        value.string = String_from_buffer(buffer, strlen(buffer));
        HashMap_put(map, Integer(key), value);
    }
    
    for (key = middle_start; key < middle_end; ++key) {
        PRN(check middle)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            uint64_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%" PRIu64 "] != key [%" PRIu64 "]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    for (key = middle_start; key < middle_end; ++key) {
        PRN(prune middle)
        if (key % 5 == 0) {
            HashMap_remove(map, Integer(key));
        }
    }
    
    for (key = middle_start; key < middle_end; ++key) {
        PRN(check pruned middle)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0 && key % 5 != 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            uint64_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%" PRIu64 "] != key [%" PRIu64 "]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    uint64_t high_start = 18446744073707551614u;
    
    for (key = high_start; key >= high_start; key += 3) {
        PRN(build high)
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, key);
        struct Any value;
        value.type = StringType;
        value.string = String_from_buffer(buffer, strlen(buffer));
        HashMap_put(map, Integer(key), value);
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN(check high)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            uint64_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%" PRIu64 "] != key [%" PRIu64 "]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN(prune high)
        if (key % 5 == 0) {
            HashMap_remove(map, Integer(key));
        }
    }
    
    for (key = high_start; key >= high_start; ++key) {
        PRN(check pruned high)
        struct Any value = HashMap_get(map, Integer(key));
        if (key % 3 == 0 && key % 5 != 0) {
            if (value.type != StringType) {
                fail("Wrong type: %d", value.type);
            }
            uint64_t parsed = parse_uint64(value.string->value, value.string->length, 10);
            if (parsed != key) {
                fail("parsed [%" PRIu64 "] != key [%" PRIu64 "]", parsed, key);
            }
        } else {
            if (value.type != NoneType) {
                fail("Wrong type: %d", value.type);
            }
        }
    }
}

void hashes() {
    printf("%" PRIu64 "\n", int64_hash(1999998));
    printf("%" PRIu64 "\n", int64_hash(-1999999));
}

#define TEST(name) if (!name()) { fprintf(stderr, "Test case " #name " failed.\n"); exit(1); }

int main() {
    hashes();
    hash_map();
    printf("Success.\n");
}
