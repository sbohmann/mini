class Type:
    def __init__(self, name: str):
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


class Array(Type):
    def __init__(self, element_type: Type):
        Type.__init__(self, 'Array<' + element_type.name + '>')
        self.element_type = element_type


class Struct(Type):
    def __init__(self, name: str):
        Type.__init__(self, name)
        self.fields = []
        self.fieldNames = set()

    def field(self, name, type_: Type):
        if name in self.fieldNames:
            raise ValueError("Duplicate field name [" + name + '] on struct ' + self.name)
        self.fields.append(Field(name, type_))


class String(Type):
    def __init__(self):
        Type.__init__(self, 'String')


class Field:
    def __init__(self, name: str, type_: Type):
        self.name = name
        self.type_ = type_
