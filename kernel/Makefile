CC=		gcc
CFLAGS=		-Wall -isystem ../include
CKFLAGS=	-nostdlib -nostartfiles -nodefaultlibs

OBJS=		kernel.o proc.o

.PHONY: all
all: kernel

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: kernel
kernel: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(CKFLAGS) -o $@ -c $<
