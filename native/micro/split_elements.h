#pragma once

#include <stdint.h>

#include <minic/elements/element.h>

struct SplitElements {
    size_t size;
    struct Elements *data;
    struct Element *separators;
};

struct ElementsList;

void SplitElements_delete(struct SplitElements *instance);

struct ElementQueue;

struct SplitElements *SplitElements_by_comma(struct ElementQueue *queue);

struct SplitElements *SplitElements_by_operator(struct ElementQueue *queue, const char *text);

struct SplitElements *SplitElements_by_predicate(struct ElementQueue *queue, bool (*predicate)(const struct Element*));

struct SplitElements *SplitElements_by_line(struct ElementQueue *queue);
