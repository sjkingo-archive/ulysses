#!/bin/bash

if [ "$1" = "gdb" ]; then
    debug="-s -S"
fi

qemu $debug \
    -d cpu_reset \
    -fda arch/x86/boot/grub.img \
    -hda fat:arch/x86/boot \
    -boot a \
    -m 8 \
    -serial "file:log.txt" \
    -serial tcp:localhost:5555,server,nowait \
    &

if [ "$debug" != "" ]; then
    sleep 1 # to let qemu listen
    gdb -x gdb-startup
fi
