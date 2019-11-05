import os

import jinja2

output_directory = os.path.join('..', 'native', 'generated')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class ListGenerator():
    def __init__(self, type):
        self.type = type
        self.struct = False
        self.system_include = None
        self.local_include = None

    def run(self):
        self.name = self.type[0].upper() + self.type[1:]
        self.type_name = 'struct ' + self.type if self.struct else type
        self.value_type = 'struct ' + self.type + ' *' if self.struct else type
        self.value_type_prefix = self.value_type if self.struct else self.value_type + ' '
        self.file_name = self.name.lower() + '_list'
        self._render('h')
        self._render('c')

    def _render(self, suffix):
        template = template_environment.get_template('list.' + suffix)
        path = os.path.join(output_directory, self.file_name + '.' + suffix)
        open(path, 'w').write(template.render(
            name=self.name,
            type=self.type_name,
            value=self.value_type,
            prefix=self.value_type_prefix,
            file=self.file_name,
            system_include=self.system_include,
            local_include=self.local_include))


def generate_token_list():
    generator = ListGenerator('Token')
    generator.struct = True
    generator.local_include = '../tokenizer.h'
    generator.run()


generate_token_list()
