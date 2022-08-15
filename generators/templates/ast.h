#pragma once

struct {{name}} {
{%- for fieldname in fields %}{% set field = fields[fieldname] %}
    {{field.type}}{% if field.elementType %}<{{field.elementType}}>{% endif %} {{fieldname}};
{% endfor -%}
};

