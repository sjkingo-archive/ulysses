
global loader
global mboot

extern _kmain
extern code
extern bss
extern end

; multiboot header
MODULEALIGN equ 1 << 0 ; do alignment on page boundaries
MEMINFO equ 1 << 1 ; memory map
FLAGS equ MODULEALIGN | MEMINFO ; multiboot flag
MAGIC equ 0x1BADB002 ; bad boo 2?
CHECKSUM equ -(MAGIC + FLAGS)

[bits 32]

mboot:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

    dd mboot
    dd code
    dd bss
    dd end
    dd loader

; call the kernel
loader:
    push esp ; top of stack pointer
    push eax ; multiboot magic num
    push ebx ; multiboot info struct

    cli ; we don't want interrupts for a while yet
    call _kmain
    jmp $ ; infinite loop

