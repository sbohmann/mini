#pragma once

struct {{name}} {
{%- for fieldname in fields -%}
{%- set field = fields[fieldname] %}
{%- if field.type == 'array' %}
    {{field.elementType}} *{{fieldname}};
{%- else %}
    {{field.type}}{% if field.elementType %}<{{field.elementType}}>{% endif %} {{fieldname}};
{%- endif %}
{% endfor -%}
};

