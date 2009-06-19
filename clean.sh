#!/bin/bash
scons -Q -c
find . -name \*.orig -exec rm {} \;
rm -f boot/*
rm -f arch/x86/boot/{initrd.img,kernel}
pushd test && scons -Q -c && popd
