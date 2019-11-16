#include <stdio.h>
#include <stdbool.h>

#include "collections/hashmap.h"
#include "minic/types.h"

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
            printf("%d", (int)value.integer);
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
        case UndefinedType:
            printf("<undefined>");
            break;
        default:
            printf("<unknown>");
    }
    putchar('\n');
}

void hashmap() {
    struct HashMap *map = HashMap_create();
    size_t key = 1234567;
    print(HashMap_get(map, key));
    HashMap_put(map, key, String("Hi! :D"));
    print(HashMap_get(map, key));
    print(HashMap_get(map, key + 1));
    HashMap_put(map, key + 1, String("Ok ^^"));
    print(HashMap_get(map, key));
    print(HashMap_get(map, key + 1));
    HashMap_remove(map, key);
    print(HashMap_get(map, key));
    print(HashMap_get(map, key + 1));
}

#define TEST(name) if (!name()) { fprintf(stderr, "Test case " #name " failed.\n"); exit(1); }

int main() {
    hashmap();
    printf("Success.\n");
}
