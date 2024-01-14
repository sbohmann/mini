import os
from name_conversion import uppercase_to_underscore

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
        self.allocate = 'allocate'
        self.deallocate = 'deallocate'

    def run(self):
        self.type_name = 'struct ' + self.raw_type if self.struct else self.raw_type
        self.value_type = 'struct ' + self.raw_type + ' *' if self.struct else self.raw_type
        self.const_value_type = 'const ' + self.value_type if (self.struct and not self.mutable) else self.value_type
        self.value_type_prefix = self.const_value_type if self.struct else self.value_type + ' '
        self.value_dereference = '*' if self.struct else ''
        self.file_name = uppercase_to_underscore(self.name) + '_' + self.kind
        self._render('h')
        self._render('c')

    def _render(self, suffix):
        template = template_environment.get_template(self.kind + '.' + suffix)
        path = os.path.join(output_directory, self.file_name + '.' + suffix)
        text = template.render(name=self.name, type=self.type_name, value=self.value_type,
                               constvalue=self.const_value_type, prefix=self.value_type_prefix, file=self.file_name,
                               system_include=self.system_include, local_include=self.local_include,
                               allocate=self.allocate, deallocate=self.deallocate,
                               value_dereference=self.value_dereference)
        if os.path.isfile(path):
            file = open(path, 'r')
            existing_text = file.read()
            file.close()
            if existing_text == text:
                return
        print("Writing file [" + path + "]")
        open(path, 'w').write(text)


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


def generate_elements_list():
    generator = Generator('Elements', 'list')
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


def generate_void_pointer_list():
    generator = Generator('void *', 'list')
    generator.name = 'VoidPointer'
    generator.allocate = 'allocate_unmanaged'
    generator.deallocate = 'deallocate_unmanaged'
    generator.run()


def generate_statement_list():
    generator = Generator('Statement', 'list')
    generator.struct = True
    generator.local_include = 'ast/statement.h'
    generator.run()


def generate_expression_list():
    generator = Generator('Expression', 'list')
    generator.struct = True
    generator.local_include = 'ast/expression.h'
    generator.run()


generate_token_list()
generate_token_queue()
generate_element_list()
generate_elements_list()
generate_string_list()
generate_integer_list()
generate_void_pointer_list()
generate_statement_list()
generate_expression_list()
