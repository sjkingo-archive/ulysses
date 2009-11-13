#!/bin/bash

if [ ! -f "empty.img" ] ; then
    echo "empty.img doesn't exist in `pwd`"
    exit 1
fi

sudo losetup /dev/loop0 empty.img
sudo mke2fs -t ext2 -O none -L empty /dev/loop0
sudo losetup -d /dev/loop0
