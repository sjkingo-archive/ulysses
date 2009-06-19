#!/bin/bash
DIR="arch/x86/boot"
OUTPUT_DIR="boot"

mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -input-charset utf-8 -q -o $OUTPUT_DIR/kernel.iso $DIR
