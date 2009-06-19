#!/bin/bash
pushd .. && \
scons -Q init && \
popd && \
./make_initrd $@ && \
mv initrd.img ../arch/x86/boot/
