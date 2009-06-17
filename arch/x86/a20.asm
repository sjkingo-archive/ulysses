; arch/x86/a20.s - assembler routine to enable A20 addressing line
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

global init_a20
init_a20:
    call a20wait
    mov al, 0xAD
    out 0x64, al

    call a20wait
    mov al, 0xD0
    out 0x64, al
    
    call a20wait2
    in  al, 0x60
    push eax
    
    call a20wait
    mov al, 0xD1
    out 0x64, al
    
    call a20wait
    pop eax
    or  al, 2
    out 0x60, al
    
    call a20wait
    mov al, 0xAE
    out 0x64, al
    
    call a20wait
    ret

a20wait:
    in al, 0x64
    test al, 2
    jnz a20wait
    ret

a20wait2:
    in al, 0x64
    test al, 1
    jz a20wait2
    ret
