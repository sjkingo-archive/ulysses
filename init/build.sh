#!/bin/bash
pushd .. && scons init && popd && ./make_initrd $@ && mv initrd.img ../arch/x86/boot/
