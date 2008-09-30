
#ifndef _ISR_H
#define _ISR_H

typedef struct registers {
    unsigned int ds; /* data segment selector */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* from pusha */
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss; /* CPU gives us these */
} registers_t;

/* isr_handler()
 *  Interrupt handler, called from the assembler stubs in interrupt.s
 */
void isr_handler(registers_t regs);

/* test_interrupts()
 *  Generate interrupts 0 -> 18, in order.
 */
void test_interrupts(void);

#endif

