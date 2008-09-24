ASM=		nasm
FORMAT=		elf

LOADER=		loader.o

.PHONY: all loader clean

all: loader
loader: $(LOADER)

clean:
	rm -f $(LOADER)

%.o: %.s
	$(ASM) -f $(FORMAT) -o $@ $<
