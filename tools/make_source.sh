#!/bin/bash
REV="`hg log -l 1 --template '{rev}'`"
DIR="/sjkwi/home/sam/public_html/ulysses/r$REV"
rm -rf $DIR && ./highlight_source.py .. $DIR
