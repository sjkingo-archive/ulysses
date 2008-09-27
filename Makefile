QEMU_ARGS=	-fda boot/grub.img -hda fat:kernel -boot a -m 8

.PHONY: all debug run gdb docs

all: run

debug:
	qemu -s -S $(QEMU_ARGS)

run:
	qemu $(QEMU_ARGS)

gdb:
	gdb -x gdb-startup

docs:
	rm -rf docs && doxygen
