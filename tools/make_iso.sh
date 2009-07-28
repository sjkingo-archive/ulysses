#!/bin/bash
DIR="../arch/x86/boot"
OUTPUT_DIR="../boot"

# make the initrd first
pushd ../init && \
cp $DIR/kernel . && \
./build.sh kernel >/dev/null && \
rm -f kernel && \
popd

./make_menu.sh

echo "target remote localhost:1234" > gdb-startup
echo "symbol-file $DIR/kernel" >> gdb-startup

mkisofs -R -q  \
    -b boot/grub/stage2_eltorito \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -input-charset utf-8 \
    -o $OUTPUT_DIR/kernel.iso \
    $DIR
