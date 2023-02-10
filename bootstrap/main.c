#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "tokenizer.h"

enum State {
	verbatim,
	signature,
	name,
	end
};

struct {
} verbatim_state;

struct {
} signature_state;

struct {
} name_state;

struct {
} end_state;

int main() {
	whhile (1) {
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
