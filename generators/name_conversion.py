import re

_uppercase_part = re.compile('[A-Z][^A-Z]*')


def uppercase_to_underscore(name):
    result = ''
    for match in _uppercase_part.finditer(name):
        if match.span()[0] > 0:
            result += '_'
        result += match.group().lower()
    return result
