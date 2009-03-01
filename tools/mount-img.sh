#!/bin/bash

sudo /sbin/losetup /dev/loop0 $1
sudo mount /dev/loop0 /mnt/loop
