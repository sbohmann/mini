#pragma once

#include <stddef.h>
{% if system_include %}#include <{{system_include}}>
{%endif %}{% if local_include %}
#include "{{local_include}}"
{%endif %}
struct {{name}}List;

struct {{name}}ListElement;

struct {{name}}List * {{name}}List_create(void);

void {{name}}List_delete(struct {{name}}List * instance);

size_t {{name}}List_size(const struct {{name}}List * self);

void {{name}}List_append(struct {{name}}List * self, {{prefix}}value);

void {{name}}List_prepend(struct {{name}}List * self, {{prefix}}value);

{{type}} * {{name}}List_to_array(const struct {{name}}List * self);

struct {{name}}ListElement * {{name}}List_begin(const struct {{name}}List * self);

struct {{name}}ListElement * {{name}}List_end(const struct {{name}}List * self);

struct {{name}}ListElement * {{name}}ListIterator_next(struct {{name}}ListElement * iterator);

struct {{name}}ListElement * {{name}}ListIterator_previous(struct {{name}}ListElement * iterator);

{{constvalue}} {{name}}ListIterator_get(struct {{name}}ListElement * value);

