#include "stringbuilder.h"

enum state {
	initial,
	signature,
	name,
	semicolon
};

enum state state;

int word_part(c) {
	if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	if (c >= '0' && c <= '9') {
		return 1;
	}
	return c == '_';
}

void tokenizer_consume(char c) {
	switch (state) {
		case semicolon:
			state = initial;
		case initial:
			if (c == '@') {
				state = signature;
			}
			break;
		case signature:
			if (c == '_') {
				state = name;
			} else {
				state = initial;
			}
			break;
		case name:
			if (word_part(c)) {
				stringbuilder_append(c);
			} else if (c == ';') {
				state = semicolon;
			} else {
				state = initial;
			}
			break;
	}
}

const char * tokenizer_current_name() {
	if (state == semicolon) {
		return stringbuilder_value();
	} else {
		return 0;
	}
}
