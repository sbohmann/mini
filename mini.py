import tokenizer.tokenizer
import pprint

tokens = tokenizer.tokenizer.read_file('example.mini')
pprint.pprint(vars(tokens))
