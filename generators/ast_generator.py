import os
from name_conversion import uppercase_to_underscore

import jinja2

output_directory = os.path.join('..', 'native', 'generated', 'ast')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class Generator:
    def __init__(self, name, fields):
        self.name = name
        self.fields = fields

    def run(self):
        self.file_name = uppercase_to_underscore(self.name)
        self._render('h')
        self._render('c')

    def _render(self, suffix):
        template = template_environment.get_template('ast.' + suffix)
        path = os.path.join(output_directory, self.file_name + '.' + suffix)
        text = template.render(name=self.name, file=self.file_name)
        if os.path.isfile(path):
            file = open(path, 'r')
            existing_text = file.read()
            file.close()
            if existing_text == text:
                return
        print("Writing file [" + path + "]")
        open(path, 'w').write(text)


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
