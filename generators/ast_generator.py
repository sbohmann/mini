import io
import os

import jinja2

from code_writer import CodeWriter
from name_conversion import uppercase_to_underscore

output_directory = os.path.join('..', 'native', 'generated', 'ast')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class Generator:
    def __init__(self, name, fields):
        self._file_name = None
        self._name = name
        self._fields = fields
        self._is_union = False
        self._imports = set()

    def run(self):
        self._file_name = uppercase_to_underscore(self._name)
        self._write_header()
        self._write_code()

    def _write_header(self):
        path = os.path.join(output_directory, self._file_name + '.h')
        buffer, out = self._create_buffer(path)
        out.println(f'struct {self._name} ')
        out.block(self._write_struct_content, '};')
        text = self._prepend_imports(buffer.getvalue())
        self._write_if_necessary(path, text)

    def _write_struct_content(self, out):
        for field in self._fields:
            if field.type.is_array:
                self._imports.add('<stddef.h>')
                out.println(f"const {field.type.element_type} * const {field._name};")
                out.println(f"const size_t {field._name}Length;")
            elif field.type.is_string:
                self._imports.add('"ast/ast_types.h"')
                out.println(f'const struct ASTString {field._name};')
            else:
                out.println(f"const {field.type} {field._name};")

    def _prepend_imports(self, text):
        prefix = ['#pragma once', '']
        if len(self._imports) > 0:
            for import_name in sorted(self._imports):
                prefix.append(f'#include {import_name}')
            prefix.append('')
        return '\n'.join(prefix) + '\n' + text

    def _write_code(self):
        path = os.path.join(output_directory, self._file_name + '.c')
        buffer, out = self._create_buffer(path)
        out.println(f'#include "{self._name.lower()}.h"')
        self._write_if_necessary(path, buffer.getvalue())

    def _create_buffer(self, path):
        buffer = io.StringIO()
        return buffer, CodeWriter(buffer)

    def _write_if_necessary(self, path, text):
        if os.path.isfile(path):
            file = open(path, 'r')
            existing_text = file.read()
            file.close()
            if existing_text != text:
                print("Writing file [" + path + "]")
                open(path, 'w').write(text)


class Field:
    def __init__(self, name, field_type):
        self.name = name
        self.type = field_type


class Type:
    def __init__(self, name):
        self.name = name
        self.element_type = None
        self.is_array = False
        self.is_struct = False
        self.is_string = False

    def __str__(self):
        if self.is_array:
            return f'{self.element_type} *'
        elif self.is_struct:
            return f'struct {self.name}'
        elif self.is_string:
            return 'char *'
        else:
            return self.name


def array(element_type):
    result = Type('array')
    result.element_type = element_type
    result.is_array = True
    return result


def struct(struct_name):
    result = Type(struct_name)
    result.is_struct = True
    return result


def string():
    result = Type('string')
    result.is_string = True
    return result


Generator('Module',
          [
              Field('name', string()),
              Field('statements', array(struct('Statement')))
          ])\
    .run()

Generator('Statement',
          [

          ]) \
    .run()
