#!/bin/bash
scons -Q -j4 && \
pushd tools && \
./qemu.sh $@ && \
popd
