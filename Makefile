.PHONY: all kernel run

all: kernel

kernel:
	$(MAKE) -C kernel kernel

run:
	qemu -fda boot/grub.img -hda fat:kernel -boot a -m 8
