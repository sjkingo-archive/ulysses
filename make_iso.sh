#!/bin/bash
DIR="arch/x86/boot"
OUTPUT_DIR="boot"
mkisofs -R -q  \
    -b boot/grub/stage2_eltorito \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -input-charset utf-8 \
    -o $OUTPUT_DIR/kernel.iso \
    $DIR
