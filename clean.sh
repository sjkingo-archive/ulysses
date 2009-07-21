#!/bin/bash
scons -Q -c
find . -name \*.orig -exec rm {} \;
find . -name core -exec rm {} \;
rm -f boot/*
rm -f arch/x86/boot/{initrd.img,kernel}
rm -f arch/x86/boot/boot/grub/menu.lst
pushd test && scons -Q -c && popd
