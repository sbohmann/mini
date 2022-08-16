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
            if existing_text == text:
                return
        print(f"Existing file text length: {len(existing_text)}")
        print(f"text length: {len(text)}")
        print("Writing file [" + path + "]")
        open(path, 'w').write(text)

    def _write_header(self, out):
        out.println(f'struct {self.name} ')
        out.block(self._write_struct_content, '};')

    def _write_struct_content(self, out):
        out.println('Hi!')

    def _write_code(self, out):
        pass


def array(elementType):
    return {
        'type': 'array',
        'elementType': elementType
    }


def generate_token_list():
    generator = Generator('Module',
                          {'statements': array('Statement')})
    generator.run()


generate_token_list()
