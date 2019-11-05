#pragma once

struct {{name}}List;

struct {{name}}ListElement;

{{name}}List * {{name}}List_create();

{{name}}List * {{name}}List_delete(struct {{name}}List * list);

void {{name}}List_append(struct {{name}}List * list, {{prefix}}value);

void {{name}}List_prepend(struct {{name}}List * list, {{prefix}}value);

{{name}}ListElement * {{name}}List_begin(struct {{name}}List * list);

{{name}}ListElement * {{name}}List_end(struct {{name}}List * list);

{{name}}ListElement * {{name}}ListIterator_next({{name}}ListElement * iterator);

{{name}}ListElement * {{name}}ListIterator_previous({{name}}ListElement * iterator);

{{value}} {{name}}ListIterator_get({{name}}ListElement * value);

