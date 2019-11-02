class Position:
    def __init__(self, path, line, column):
        self.path = path
        self.line = line
        self.column = column


class Token:
    def __init__(self, text, value, position):
        self.text = text
        self.value = value
        self.position = position


def read_file(path):
    file = open(path, 'r')
    content = file.read()
    return Tokens(path, content)


class Tokens:
    def __init__(self, path, source):
        self.path = path
        self.source = source
