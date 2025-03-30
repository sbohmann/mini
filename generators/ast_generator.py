import io
import os

from code_writer import CodeWriter
from generators.type import String, Struct, Array
from name_conversion import uppercase_to_underscore

output_directory = os.path.join('..', 'native', 'generated', 'ast')
if not os.path.isdir(output_directory):
    raise ValueError('Not a directory: ' + output_directory)

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
        buffer, out = self._create_buffer()
        out.println(f'struct {self._name} ')
        out.block(self._write_struct_content, '};')
        text = self._prepend_imports(buffer.getvalue())
        self._write_if_necessary(path, text)

    def _write_struct_content(self, out):
        for field in self._fields:
            if field.type is Array:
                self._imports.add('<stddef.h>')
                out.println(f"const {field.type.element_type} * const {field.name};")
                out.println(f"const size_t {field.name}Length;")
            elif field.type is String:
                self._imports.add('"ast/ast_types.h"')
                out.println(f'const struct ASTString {field.name};')
            else:
                out.println(f"const {field.type} {field.name};")

    def _prepend_imports(self, text):
        prefix = ['#pragma once', '']
        if len(self._imports) > 0:
            for import_name in sorted(self._imports):
                prefix.append(f'#include {import_name}')
            prefix.append('')
        return '\n'.join(prefix) + '\n' + text

    def _write_code(self):
        path = os.path.join(output_directory, self._file_name + '.c')
        buffer, out = self._create_buffer()
        out.println(f'#include "{self._name.lower()}.h"')
        self._write_if_necessary(path, buffer.getvalue())

    @staticmethod
    def _create_buffer():
        buffer = io.StringIO()
        return buffer, CodeWriter(buffer)

    @staticmethod
    def _write_if_necessary(path, text):
        if os.path.isfile(path):
            file = open(path, 'r')
            existing_text = file.read()
            file.close()
            if existing_text == text:
                return
        print("Writing file [" + path + "]")
        open(path, 'w').write(text)


class Field:
    def __init__(self, name, field_type):
        self.name = name
        self.type = field_type


Generator('Module',
          [
              Field('name', String()),
              Field('statements', Array(Struct('Statement')))
          ])\
    .run()

Generator('Statement',
          [

          ]) \
    .run()
