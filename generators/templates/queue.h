#pragma once
{% if system_include %}#include <{{system_include}}>
{%endif %}{% if local_include %}
#include "{{local_include}}"
{%endif %}
struct {{name}}Queue;

struct {{name}}Queue *{{name}}Queue_create(const struct {{name}}s *list);

void {{name}}Queue_delete(struct {{name}}Queue *instance);

{{constvalue}}{{name}}Queue_peek(struct {{name}}Queue *self);

{{constvalue}}{{name}}Queue_next(struct {{name}}Queue *self);

bool {{name}}Queue_contains(struct {{name}}Queue *self, bool (*predicate)({{constvalue}}));

