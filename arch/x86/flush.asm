; arch/x86/flush.s - descriptor table flush and protected mode routines
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

bits 32
section .text

global gdt_flush
gdt_flush:
    ; get pointer to gdt struct and load it
    mov eax, [esp+4]
    lgdt [eax]

    ; load the data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush ; offset to code segment
.flush:
   ret

; Load the interrupt table - we should now have interrupts!
global idt_flush
idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

; Enter protected mode
global enter_pm
enter_pm:
    mov eax, cr0
    or eax, 1 ; PM bit
    mov cr0, eax
    jmp 0x08:.pm ; far jump into protected mode
.pm:
    ret

; Tell the CPU about our single TSS for task switching
global tss_flush
tss_flush:
    mov ax, 0x2b ; rpl of 3
    ltr ax ; load into task state reg
    ret
