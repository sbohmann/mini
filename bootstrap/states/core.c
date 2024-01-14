#include <stdio.h>
#include <stdlib.h>

#include "core.h"

enum State state;

static ReplacementCallback replacementCallback;

void states_initialize(ReplacementCallback callback) {
    state = Verbatim;
    replacementCallback = callback;
}

const char * replace(const char *name) {
    if (replacementCallback != 0) {
        const char *result = replacementCallback(name);
        if (result != 0) {
            return result;
        }
    }
    return name;
}

void process(char c) {
    switch (state) {
        case Verbatim:
            verbatim_process(c);
            break;
        case StartSignature:
            start_signature_process(c);
            break;
        case Name:
            name_process(c);
            break;
        case EndSignature:
            end_signature_process(c);
            break;
        case EndOfFile:
            fprintf(stderr, "Attempt to process character in EnfOfFile state.");
            exit(1);
    }
}

void end_of_file() {
    switch (state) {
        case Verbatim:
            break;
        case StartSignature:
            start_signature_end_of_file();
            break;
        case Name:
            name_end_of_file();
            break;
        case EndSignature:
            end_signature_end_of_file();
            break;
        case EndOfFile:
            fprintf(stderr, "Attempt to process EOF in EnfOfFile state.");
            exit(1);
    }
    state = EndOfFile;
}
