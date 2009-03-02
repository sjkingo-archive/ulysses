
[bits 32]
[section .text]

; Descriptor table flush.

global gdt_flush
global idt_flush
global enter_pm

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
idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

; Enter protected mode
enter_pm:
    cli ; not re-entrant
    mov eax, cr0
    or eax, 1 ; PM bit
    mov cr0, eax
    jmp 0x08:.pm ; far jump into protected mode
.pm:
    sti
    ret

