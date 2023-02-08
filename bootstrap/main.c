#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "tokenizer.h"

int main() {
	while (1) {
		int c = getchar();
		if (c < 0) {
			break;
		}
		tokenizer_consume(c);
		const char *current_name = tokenizer_current_name();
		if (current_name != 0) {
			printf("Found name: %s\n", current_name);
		}
	}
	if (errno != 0) {
		printf("Error: %s\n", strerror(errno));
	}
}
