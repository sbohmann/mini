import os

import jinja2

output_directory = os.path.join('..', 'native', 'generated')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class ListGenerator():
    def __init__(self, typename):
        self.raw_type = typename
        self.name = typename[0].upper() + typename[1:]
        self.struct = False
        self.system_include = None
        self.local_include = None

    def run(self):
        self.type_name = 'struct ' + self.raw_type if self.struct else self.raw_type
        self.value_type = 'struct ' + self.raw_type + ' *' if self.struct else self.raw_type
        self.value_type_prefix = self.value_type if self.struct else self.value_type + ' '
        self.value_dereference = '*' if self.struct else ''
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
            local_include=self.local_include,
            value_dereference=self.value_dereference))


def generate_token_list():
    generator = ListGenerator('Token')
    generator.struct = True
    generator.local_include = 'minic/tokenizer.h'
    generator.run()


def generate_string_list():
    generator = ListGenerator('String')
    generator.struct = True
    generator.local_include = 'core/string.h'
    generator.run()


def generate_integer_list():
    generator = ListGenerator('int64_t')
    generator.name = 'Int'
    generator.system_include = 'stdint.h'
    generator.run()


generate_token_list()
generate_string_list()
generate_integer_list()
