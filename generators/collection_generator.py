import os

import jinja2

output_directory = os.path.join('..', 'native', 'generated')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


class Generator:
    def __init__(self, typename, kind):
        self.raw_type = typename
        self.kind = kind
        self.name = typename[0].upper() + typename[1:]
        self.struct = False
        self.mutable = False
        self.system_include = None
        self.local_include = None

    def run(self):
        self.type_name = 'struct ' + self.raw_type if self.struct else self.raw_type
        self.value_type = 'struct ' + self.raw_type + ' *' if self.struct else self.raw_type
        self.const_value_type = 'const ' + self.value_type if (self.struct and not self.mutable) else self.value_type
        self.value_type_prefix = self.const_value_type if self.struct else self.value_type + ' '
        self.value_dereference = '*' if self.struct else ''
        self.file_name = self.name.lower() + '_' + self.kind
        self._render('h')
        self._render('c')

    def _render(self, suffix):
        template = template_environment.get_template(self.kind + '.' + suffix)
        path = os.path.join(output_directory, self.file_name + '.' + suffix)
        open(path, 'w').write(template.render(
            name=self.name,
            type=self.type_name,
            value=self.value_type,
            constvalue=self.const_value_type,
            prefix=self.value_type_prefix,
            file=self.file_name,
            system_include=self.system_include,
            local_include=self.local_include,
            value_dereference=self.value_dereference))


def generate_token_list():
    generator = Generator('Token', 'list')
    generator.struct = True
    generator.local_include = 'minic/tokens/token.h'
    generator.run()


def generate_token_queue():
    generator = Generator('Token', 'queue')
    generator.struct = True
    generator.local_include = 'minic/tokens/token.h'
    generator.run()


def generate_element_list():
    generator = Generator('Element', 'list')
    generator.struct = True
    generator.local_include = 'minic/elements/element.h'
    generator.run()


def generate_element_queue():
    generator = Generator('Element', 'queue')
    generator.struct = True
    generator.local_include = 'minic/elements/element.h'
    generator.run()


def generate_string_list():
    generator = Generator('String', 'list')
    generator.struct = True
    generator.local_include = 'core/string.h'
    generator.run()


def generate_integer_list():
    generator = Generator('int64_t', 'list')
    generator.name = 'Int'
    generator.system_include = 'stdint.h'
    generator.run()


def generate_variable_list():
    generator = Generator('Variable', 'list')
    generator.struct = True
    generator.mutable = True
    generator.local_include = 'minic/any.h'
    generator.run()


generate_token_list()
generate_token_queue()
generate_element_list()
generate_element_queue()
generate_string_list()
generate_integer_list()
generate_variable_list()
