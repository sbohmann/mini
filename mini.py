import tokenizer
import pprint

tokens = tokenizer.read_file('mini.py')
pprint.pprint(vars(tokens))
