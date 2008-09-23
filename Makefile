
KERNEL=		kernel.bin
BOOTDIR=	boot
KERNDIR=	kernel

.PHONY: all
all: link

.PHONY: kernel
kernel:
	$(MAKE) -C $(KERNDIR) kernel
	$(MAKE) -C $(BOOTDIR) loader

.PHONY: link
link: kernel
	ld -T $(BOOTDIR)/linker.ld -o $(KERNEL) $(KERNDIR)/kernel.o $(BOOTDIR)/loader.o

.PHONY: clean
clean:
	$(MAKE) -C $(KERNDIR) clean
	$(MAKE) -C $(BOOTDIR) clean
	rm -f $(KERNEL)
