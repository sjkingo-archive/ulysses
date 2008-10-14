CC=		gcc
LD=		ld
ASM=		nasm
CFLAGS=		-m32 -g -Wall -Wextra -isystem include
LDFLAGS=	-melf_i386 -nostdlib -nostartfiles -nodefaultlibs
ASMFLAGS=	-f elf

OBJS=		kernel/main.o kernel/kprintf.o kernel/proc.o kernel/shutdown.o kernel/vt.o lib/itoa.o lib/string.o
OBJS_ARCH=	arch/x86/startup.o arch/x86/halt.o arch/x86/util.o arch/x86/timer.o arch/x86/gdt.o arch/x86/isr/idt.o arch/x86/isr/interrupt.o arch/x86/isr/isr.o arch/x86/flush.o arch/x86/paging.o arch/x86/screen.o

LOADEROBJ=	arch/x86/loader.o
LINKER=		arch/x86/linker.ld

QEMU_ARGS=	-fda arch/x86/grub.img -hda fat:kernel -boot a -m 8

.PHONY: all clean debug run gdb docs

all: kernel/kernel

kernel/kernel: $(OBJS) $(OBJS_ARCH) $(LOADEROBJ)
	$(LD) $(LDFLAGS) -T $(LINKER) -o kernel/kernel $(LOADEROBJ) $(OBJS) $(OBJS_ARCH)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.s
	$(ASM) $(ASMFLAGS) -o $@ $<

clean:
	rm -f kernel/*.o lib/*.o arch/x86/*.o arch/x86/isr/*.o kernel/kernel

debug:
	qemu -s -S $(QEMU_ARGS)

run:
	qemu $(QEMU_ARGS)

gdb:
	gdb -x gdb-startup

docs:
	rm -rf docs && doxygen
