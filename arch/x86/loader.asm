; arch/x86/loader.s - bootstraps the C kernel from multiboot loader
; part of Ulysses, a tiny operating system
;
; Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
; 
; This program is free software: you can redistribute it and/or modify it
; under the terms of the GNU General Public License as published by the Free
; Software Foundation, either version 3 of the License, or (at your option)
; any later version.
; 
; This program is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
; FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
; more details.
; 
; You should have received a copy of the GNU General Public License along
; with this program.  If not, see <http://www.gnu.org/licenses/>.

global loader
global mboot

extern _kmain_x86
extern code
extern bss
extern end

; multiboot header
MODULEALIGN equ 1 << 0 ; do alignment on page boundaries
MEMINFO equ 1 << 1 ; memory map
FLAGS equ MODULEALIGN | MEMINFO ; multiboot flag
MAGIC equ 0x1BADB002 ; bad boo 2?
CHECKSUM equ -(MAGIC + FLAGS)

bits 32

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
    call _kmain_x86
    jmp $ ; we should never, ever, ever get here but busy loop if we do
