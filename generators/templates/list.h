#pragma once
{% if system_include %}
#include <{{system_include}}>
{%endif %}{% if local_include %}
#include "{{local_include}}"
{%endif %}
struct {{name}}List;

struct {{name}}ListElement;

struct {{name}}List * {{name}}List_create();

struct {{name}}List * {{name}}List_delete(struct {{name}}List * list);

void {{name}}List_append(struct {{name}}List * list, {{prefix}}value);

void {{name}}List_prepend(struct {{name}}List * list, {{prefix}}value);

struct {{name}}ListElement * {{name}}List_begin(struct {{name}}List * list);

struct {{name}}ListElement * {{name}}List_end(struct {{name}}List * list);

struct {{name}}ListElement * {{name}}ListIterator_next(struct {{name}}ListElement * iterator);

struct {{name}}ListElement * {{name}}ListIterator_previous(struct {{name}}ListElement * iterator);

{{value}} {{name}}ListIterator_get(struct {{name}}ListElement * value);

