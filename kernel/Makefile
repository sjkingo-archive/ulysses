CC=		gcc
LD=		ld
CFLAGS=		-Wall -Wextra -isystem ../include
LDFLAGS=	-nostdlib -nostartfiles -nodefaultlibs

OBJS=		main.o kprintf.o proc.o shutdown.o
LOADEROBJ=	../boot/loader.o

.PHONY: all clean loader kernel

all: loader kernel

clean:
	rm -f *.o kernel
	$(MAKE) -C ../boot clean

loader:
	$(MAKE) -C ../boot loader

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -T linker.ld -o kernel $(LOADEROBJ) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
