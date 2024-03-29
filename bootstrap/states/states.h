#pragma once

#include "configuration.h"

enum State {
    Verbatim,
    StartSignature,
    Name,
    EndSignature,
    EndOfFile
};

extern enum State state;

typedef const char * (*ReplacementCallback)(const char *c);

void states_initialize(ReplacementCallback replacementCallback);

const char * replace(const char *name);

void process(char c);

void end_of_file();
