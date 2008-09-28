CC=		gcc
LD=		ld
CFLAGS=		-m32 -g -Wall -Wextra -Werror -isystem ../include
LDFLAGS=	-melf_i386 -nostdlib -nostartfiles -nodefaultlibs

OBJS=		main.o kprintf.o proc.o shutdown.o ../lib/itoa.o
LOADEROBJ=	../boot/loader.o
LINKER=		../arch/x86/linker.ld

.PHONY: all clean loader kernel

all: loader kernel

clean:
	rm -f *.o $(OBJS) kernel
	$(MAKE) -C ../boot clean

loader:
	$(MAKE) -C ../boot loader

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -T $(LINKER) -o kernel $(LOADEROBJ) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
