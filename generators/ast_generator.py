import io
import os

import jinja2

from code_writer import CodeWriter
from name_conversion import uppercase_to_underscore

output_directory = os.path.join('..', 'native', 'generated', 'ast')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class Generator:
    def __init__(self, name, fields):
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
            if field.is_array:
                out.println(f"{field.element_type} *{field.name};")
            else:
                out.println(f"{field.type} {field.name};")

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


def array_field(name, element_type):
    field_type = Type('array')
    field_type.element_type = element_type
    field_type.is_array = True
    return Field(name, field_type)


def struct_field(name, struct_name):
    field_type = Type(struct_name)
    field_type.is_struct = True
    return Field(name, field_type)


def generate_token_list():
    generator = Generator('Module',
                          [array_field('statements', 'Statement')])
    generator.run()


generate_token_list()
