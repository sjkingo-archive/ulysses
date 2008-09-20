CC=		gcc
CFLAGS=		-Wall
CKFLAGS=	-nostdlib -nostartfiles -nodefaultlibs

OBJS=		kernel.o

.PHONY: all
all: kernel

.PHONY: clean
clean:
	rm -f $(OBJS)

.PHONY: kernel
kernel: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(CKFLAGS) -o $@ -c $<
