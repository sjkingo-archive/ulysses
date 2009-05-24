#!/bin/bash
scons && ./make_initrd $@ && mv initrd.img ../arch/x86/boot/
