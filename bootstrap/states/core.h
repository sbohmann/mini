#pragma once

#include "states.h"

void enter_verbatim_state();
void verbatim_process(char c);

void enter_start_signature_state();
void start_signature_process(char c);
void start_signature_end_of_file();

void enter_name_state(char c);
void name_process(char c);
void name_end_of_file();

void enter_end_signature_state();
void end_signature_process(char c);
void end_signature_end_of_file();
