#!/bin/bash
REV="`hg log -l 1 --template '{rev}'`"
TOP_DIR="/sjkwi/home/sam/sjkingston.com/ulysses_source"
DIR="$TOP_DIR/r$REV"

rm -rf $DIR && 
./highlight_source.py .. $DIR &&
rm -f $TOP_DIR/latest &&
pushd $TOP_DIR &&
ln -s r$REV latest &&
popd
