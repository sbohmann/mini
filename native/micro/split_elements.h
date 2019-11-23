#pragma once

#include <stdint.h>

#include <minic/elements/element.h>

struct SplitElements {
    size_t size;
    struct Elements *data;
};

struct ElementsList;

struct SplitElements *SplitElements_from_list(const struct ElementsList *list);

void SplitElements_delete(struct SplitElements *instance);

struct SplitElements * SplitElements_by_comma(struct Elements *elements);
