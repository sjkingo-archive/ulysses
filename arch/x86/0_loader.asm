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
global magic
global mdb

extern _kmain_x86

; place the multiboot header in a separate section
section .__mbHeader
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
MODULEALIGN equ 1 << 0 ; do alignment on page boundaries
MEMINFO equ 1 << 1 ; memory map
FLAGS equ MODULEALIGN | MEMINFO ; multiboot flag
MAGIC equ 0x1BADB002 ; bad boo 2?
CHECKSUM equ -(MAGIC + FLAGS)

section .text

STACKSIZE equ 0x4000

loader:
    mov esp, stack + STACKSIZE ; set up the stack
    push esp ; stack pointer (arg2)
    push eax ; multiboot magic (arg1)
    push ebx ; multiboot struct (arg0)
    
    cli ; we don't want interrupts for a while yet
    call _kmain_x86
    cli
.hang:
    hlt
    jmp .hang

section .bss
align 4
stack:  resb STACKSIZE
magic:  resd 1
mdb:    resd 1
