#!/bin/bash
REV="`hg log -l 1 --template '{rev}'`"
TOP_DIR="/sjkwi/home/sam/public_html/ulysses"
DIR="$TOP_DIR/r$REV"

rm -rf $DIR && 
./highlight_source.py .. $DIR &&
rm -f $TOP_DIR/latest &&
ln -s $DIR $TOP_DIR/latest
