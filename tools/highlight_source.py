#!/usr/bin/env python

import os

from pygments import highlight
from pygments.lexers import get_lexer_for_filename, NasmLexer
from pygments.formatters import HtmlFormatter

root = '..'
dirs = ['arch/x86', 'lib', 'kernel', 'include', 'include/ulysses', 'init', 
        'tools', 'test', 'test/heap']
exts = ['.c', '.asm', '.h', '.sh']

def output_file(src, dest, title, lexer):
    fp = open(src, 'r')
    out = open(dest, 'w')
    code = fp.read()

    formatter = HtmlFormatter(
            full=True, # inline CSS and html
            anchorlinenos=True, # add anchors to line numbers
            lineanchors='line', # prefix to <a name="..">
            linenos='table', # use a 2-col table
            encoding='utf-8',
            title=title,
    )
    result = highlight(code, lexer, formatter, out)

    out.close()
    fp.close()

def make_source(root, dirs, dest_prefix):
    src_prefix = root
    os.mkdir(dest_prefix)

    for d in dirs:
        src_dir = os.path.join(root, d)
        dest_dir = os.path.join(dest_prefix, d)
        for f in os.listdir(src_dir):
            if os.path.splitext(f)[1] not in exts:
                continue
            try: os.makedirs(dest_dir)
            except: pass

            dest_file = os.path.join(dest_dir, f + '.html')
            in_file = os.path.join(src_dir, f)

            lexer = get_lexer_for_filename(f)

            print 'using %s for source %s to %s' % (str(lexer), in_file, 
                    dest_file)
            output_file(in_file, dest_file, os.path.join(d, f) + ', Ulysses', 
                    lexer)


if __name__ == '__main__':
    import sys
    if len(sys.argv) != 3:
        print >> sys.stderr, 'Usage: %s root dest_dir' % sys.argv[0]
        exit(1)
    make_source(sys.argv[1], dirs, sys.argv[2])

