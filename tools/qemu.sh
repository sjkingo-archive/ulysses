#!/bin/bash

./make_iso.sh || exit $?

if [ "$1" = "gdb" ]; then
    debug="-s -S"
fi

qemu $debug \
    -d cpu_reset \
    -cdrom ../boot/kernel.iso \
    -boot d \
    -m 8 \
    -serial "file:../boot/log.txt" \
    &

if [ "$debug" != "" ]; then
    sleep 1 # to let qemu listen
    gdb -x gdb-startup
fi
