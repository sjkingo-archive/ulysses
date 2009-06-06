#!/bin/bash
scons -Q -c
find . -name \*.orig -exec rm {} \;
rm -f log.txt
pushd test && scons -Q -c && popd
