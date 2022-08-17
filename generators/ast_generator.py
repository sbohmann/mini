import io
import os

import jinja2

from code_writer import CodeWriter
from name_conversion import uppercase_to_underscore

output_directory = os.path.join('..', 'native', 'generated', 'ast')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class Generator:
    def __init__(self, name, fields):
        self.file_name = None
        self.name = name
        self.fields = fields

    def run(self):
        self.file_name = uppercase_to_underscore(self.name)
        self._render('h', self._write_header)
        self._render('c', self._write_code)

    def _render(self, suffix, action):
        path = os.path.join(output_directory, self.file_name + '.' + suffix)
        buffer = io.StringIO()
        out = CodeWriter(buffer)
        action(out)
        text = buffer.getvalue()
        if os.path.isfile(path):
            file = open(path, 'r')
            existing_text = file.read()
            file.close()
            if existing_text != text:
                print("Writing file [" + path + "]")
                open(path, 'w').write(text)

    def _write_header(self, out):
        out.println(f'struct {self.name} ')
        out.block(self._write_struct_content, '};')

    def _write_struct_content(self, out):
        for field in self.fields:
            if field.type.is_array:
                out.println(f"const {field.type.element_type} * const {field.name};")
            elif field.type.is_string:
                out.println(f'const char * const {field.name};')
            else:
                out.println(f"const {field.type} {field.name};")

    def _write_code(self, out):
        pass


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


def generate_token_list():
    generator = Generator('Module',
                          [
                              Field('name', string()),
                              Field('statements', array(struct('Statement')))
                          ])
    generator.run()


generate_token_list()
