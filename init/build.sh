#!/bin/bash
pushd .. && \
scons -Q init && \
popd && \
pushd files && \
../make_initrd * && \
mv initrd.img ../../arch/x86/boot/ && \
popd
