#!/bin/bash
scons -Q && \
pushd tools && \
./qemu.sh && \
popd
