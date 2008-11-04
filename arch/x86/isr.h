
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

/* These are the interrupt handler stubs. They are declared in interrupt.s.
 * This is very messy, since the CPU has the possibility of generating 32
 * interrupts (and will triple fault if no handler exists for them).
 */
extern void isr0(); /* div by zero */
extern void isr1(); /* debug */
extern void isr2(); /* non-maskable interrupt */
extern void isr3(); /* breakpoint */
extern void isr4(); /* overflow */
extern void isr5(); /* out of bounds */
extern void isr6(); /* invalid opcode */
extern void isr7(); /* no coprocessor */
extern void isr8(); /* double fault (w/ error code) */
extern void isr9(); /* coprocessor segment overrun */
extern void isr10(); /* bad tss (w/ error code) */
extern void isr11(); /* segment not present (w/ error code) */
extern void isr12(); /* stack fault (w/ error code) */
extern void isr13(); /* general protection fault (w/ error code) */
extern void isr14(); /* page fault (w/ error code) */
extern void isr15(); /* uknown interrupt */
extern void isr16(); /* coprocessor fault */
extern void isr17(); /* alignment check */
extern void isr18(); /* machine exception */

/* the rest are reserved... */
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

/* and anything 32 -> 256 are configurable by us */

/* Programmable IRQ vectors */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* phew. now look in interrupt.s... */

#endif

