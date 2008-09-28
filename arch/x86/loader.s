
global _loader
extern _kmain

; multiboot header
MODULEALIGN equ 1 << 0 ; do alignment on page boundaries
MEMINFO equ 1 << 0 ; memory map
FLAGS equ MODULEALIGN | MEMINFO ; multiboot flag
MAGIC equ 0x1BADB002 ; bad boo 2?
CHECKSUM equ -(MAGIC + FLAGS)

section .text
align 4
MultiBootHeader:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

STACKSIZE equ 0x4000 ; 16 KB of kernel stack

; call the kernel
_loader:
    mov esp, stack+STACKSIZE ; we now have a stack
    push eax ; multiboot magic num
    push ebx ; multiboot info struct

    call _kmain
    cli ; disable interrupts
    hlt ; halt the CPU

section .bss
align 32
stack:
    resb STACKSIZE ; 16 KB stack of quadword boundary

