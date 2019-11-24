#pragma once

#include <generated/element_queue.h>

void print_value(struct Any value);

struct Variables;

void print(struct Variables *context, struct ElementQueue *arguments);
