#include "configuration.h"

enum State {
    Verbatim,
    StartSignature,
    Name,
    EndSignature
};

enum State state;

void enter_verbatim_state();
void verbatim_process(char c);

void enter_start_signature_state();
void start_signature_process(char c);
void start_signature_end_of_file(char c);

void enter_name_state();
void name_process(char c);
void name_end_of_file(char c);

void enter_end_signature_state();
void end_signature_process(char c);
void end_signature_end_of_file(char c);

static void initialize() {
    state = Verbatim;
}

static void process(char c) {
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
    }
}

static void end_of_file() {
    switch (state) {
        case Verbatim:
            verbatim_end_of_file();
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
    }
}
