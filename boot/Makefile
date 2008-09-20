
ASM=		nasm
FORMAT=		elf

LOADER=		loader.o

.PHONY: loader
loader: $(LOADER)

%.o: %.s
	$(ASM) -f $(FORMAT) -o $@ $<

.PHONY: clean
clean:
	rm -f $(LOADER)
