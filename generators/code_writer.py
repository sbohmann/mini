class CodeWriter:
    def __init__(self, out):
        self._out = out
        self._line_start = True
        self._indentation = 0
        self._indentation_step = 4

    def println(self, text):
        self._indent()
        self._out.write(text)
        self._out.write('\n')
        self._line_start = True

    def print(self, text):
        self._indent()
        self._out.write(text)
        self._line_start = False

    def indented(self, action):
        self._indentation += 1
        try:
            action(self)
        finally:
            self._indentation -= 1

    def block(self, action, end='}'):
        self.println('{')
        self.indented(action)
        self.println(end)

    def _indent(self):
        if self._line_start:
            for n in range(self._indentation * self._indentation_step):
                self._out.write(' ')
