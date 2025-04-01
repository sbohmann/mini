class Type:
    def __init__(self, name: str):
        self.name = name
        self.element_type = None

    def __str__(self):
        return self.name


class Array(Type):
    def __init__(self, element_type: Type):
        Type.__init__(self, 'Array<' + str(element_type) + '>')
        self.element_type = element_type

    def __str__(self):
        return f'{self.element_type} *'


class Struct(Type):
    def __init__(self, name: str):
        Type.__init__(self, name)
        self.fields = []
        self.fieldNames = set()

    def __str__(self):
        return f'struct {self.name}'

    def field(self, name, type_: Type):
        if name in self.fieldNames:
            raise ValueError("Duplicate field name [" + name + '] on struct ' + self.name)
        self.fields.append(Field(name, type_))


class String(Type):
    def __init__(self):
        Type.__init__(self, 'String')

    def __str__(self):
        return 'char *'


class Field:
    def __init__(self, name: str, type_: Type):
        self.name = name
        self.type_ = type_
