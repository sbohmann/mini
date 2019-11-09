#include <stdio.h>
#include <stdbool.h>

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

#define TEST(name) if (!name()) { fprintf(stderr, "Test case " #name " failed.\n"); exit(1); }

int main() {
    duplication();
    printf("Success.\n");
}
