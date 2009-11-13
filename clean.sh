#!/bin/bash
scons -Q -c
find . -name \*.orig -print | xargs rm -f
find . -name core -print | xargs rm -f
find . -name *.o -print | xargs rm -f
rm -f boot/*
rm -f arch/x86/boot/{initrd.img,kernel}
rm -f arch/x86/boot/boot/grub/menu.lst
pushd test && scons -Q -c && popd
