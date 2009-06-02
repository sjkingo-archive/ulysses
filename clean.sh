#!/bin/bash
scons -Q -c
find . -name \*.orig -exec rm {} \;
