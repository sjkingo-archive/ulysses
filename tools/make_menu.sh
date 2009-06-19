#!/bin/bash
FILE="../arch/x86/boot/boot/grub/menu.lst"
INITRD="../arch/x86/boot/initrd.img"

rm -f $FILE && touch $FILE
VER=`grep '#define VERSION_NUM' ../include/ulysses/kernel.h | \
        cut -d ' ' -f 3 | awk '{ gsub(/\"/,""); print }'`

echo "default 0" >> $FILE
echo "timeout 5" >> $FILE

echo "title Ulysses $VER" >> $FILE
echo "kernel /kernel" >> $FILE

if [ -f "$INITRD" ] ; then
    echo "module /initrd.img" >> $FILE
fi
