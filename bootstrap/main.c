#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "configuration.h"

enum State {
	Verbatim,
	StartSignature,
	Name,
	EndSignature
};

enum State state;

const size_t buffer_size = 256;
char buffer[buffer_size];

struct {
	size_t offset;
} signature_state;

struct {
} name_state;

struct {
} end_state;

static void init() {
	state = Verbatim;
}

static void process(char c) {
	switch (state) {
		case Verbatim:
			if (c == signature[0]) {
				state = StartSignature;
				signature_state.offset = 0;
			} else {
				putchar(c);
			}
	}
}

int main() {
	init();
	while (true) {
		char c = getchar();
		if (c < 0) {
			if (errno != 0) {
				fprintf(stderr, "Error while reading input: %s\n", strerror(errno));
				exit(1);
			} else {
				break;
			}
		}
		process(c);
	}
}
