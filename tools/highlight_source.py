#!/usr/bin/env python

import os

from pygments import highlight
from pygments.lexers import get_lexer_for_filename, NasmLexer
from pygments.formatters import HtmlFormatter

exts = ['.c', '.s', '.sh', '.h']

def output_file(src, dest, title, lexer):
    fp = open(src, 'r')
    out = open(dest, 'w')
    code = fp.read()

    formatter = HtmlFormatter(full=True, linenos='table', title=title)
    result = highlight(code, lexer, formatter, out)

    out.close()
    fp.close()

def make_source(root, dirs):
    src_prefix = root
    dest_prefix = os.path.join('.', 'html')
    os.mkdir(dest_prefix)

    for d in dirs:
        src_dir = os.path.join(root, d)
        dest_dir = os.path.join(dest_prefix, d)
        for f in os.listdir(src_dir):
            try:
                os.makedirs(dest_dir)
            except OSError:
                pass
            if os.path.splitext(f)[1] not in exts:
                continue

            dest_file = os.path.join(dest_dir, f + '.html')
            in_file = os.path.join(src_dir, f)

            lexer = get_lexer_for_filename(f)
            if f.endswith('.s'):
                lexer = NasmLexer()

            print 'using %s for source %s to %s' % (str(lexer), in_file, dest_file)
            output_file(in_file, dest_file, os.path.join(d, f) + ', Ulysses', lexer)

if __name__ == '__main__':
    make_source('..', ['arch/x86', 'lib', 'kernel', 'include', 'include/ulysses'])
