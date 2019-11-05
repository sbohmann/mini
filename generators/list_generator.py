import os

import jinja2

output_directory = os.path.join('..', 'native', 'generated')
template_environment = jinja2.Environment(loader=jinja2.FileSystemLoader(searchpath='templates'))


def generate_list(type, struct=False):
    name = type[0].upper() + type[1:]
    type_name = 'struct ' + type if struct else type
    value_type = 'struct ' + type + ' *' if struct else type
    value_type_prefix = value_type if struct else value_type + ' '
    file_name = name.lower() + '_list'

    def render(suffix):
        template = template_environment.get_template('list.' + suffix)
        path = os.path.join(output_directory, file_name + '.' + suffix)
        open(path, 'w').write(template.render(
            name=name,
            type=type_name,
            value=value_type,
            prefix=value_type_prefix,
            file=file_name))

    render('h', )
    render('c')


generate_list('Token', True)
