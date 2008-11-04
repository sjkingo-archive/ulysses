
#ifndef _ISR_H
#define _ISR_H

/* Since we remap IRQs to interrupt numbers >= 32, assign a useful name to
 * each of them.
 */
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct registers {
    unsigned int ds; /* data segment selector */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* from pusha */
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss; /* CPU gives us these */
} registers_t;

/* Callback for IRQ handling */
typedef void (*isr_t)(registers_t);

/* Interrupt vectors */
isr_t interrupt_handlers[256];

/* isr_handler()
 *  Interrupt handler, called from the assembler stubs in interrupt.s
 */
void isr_handler(registers_t regs);

/* test_interrupts()
 *  Generate interrupts 0 -> 18, in order.
 */
void test_interrupts(void);

/* register_interrupt_handler()
 *  Register the given function as a vector for IRQ n.
 */
void register_interrupt_handler(unsigned char n, isr_t handler);

/* irq_handler()
 *  IRQ interrupt handler, called from the assembler stubs in interrupt.s
 */
void irq_handler(registers_t regs);

#endif

