#ifndef _ULYSSES_ISR_H
#define _ULYSSES_ISR_H

/* Interrupt service routines. This declares the handling for interrupts
 * generated after we load the IDT. We handle both CPU-generated interrupts
 * (vectors 0 to 31) and IRQ-generated hardware interrupts (vectors 32 to 47)
 * here.
 *
 * Since 8086 maps the IRQ table over the top of the CPU interrupt vectors
 * by default, we need to remap the IRQ interrupts to vectors 32 and onward
 * so not to mask any CPU-generated interrupts.
 *
 * In general, IRQ-generated interrupts are safe to ignore, however ignoring
 * a CPU-generated interrupt can easily lead to certain protection mechanisms
 * (set up in the GDT) to be violated. This will cause the CPU-interrupt to be
 * closely followed by a GPF (int 13).
 */

typedef struct registers {
    unsigned int ds; /* data segment selector */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; /* from pusha */
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss; /* CPU gives us these */
} registers_t;

/* Callback for interrupt handler */
typedef void (*isr_t)(registers_t);

/* Interrupt vectors */
isr_t interrupt_handlers[256];

/* isr_handler()
 *  Interrupt handler, called from the assembler stubs in interrupt.s
 */
void isr_handler(registers_t regs);

/* irq_handler()
 *  IRQ interrupt handler, called from the assembler stubs in interrupt.s
 */
void irq_handler(registers_t regs);

/* CPU-generated interrupt handler stubs; declared in interrupt.s */
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

/* Vectors 19 to 31 are reserved for __future__ by Intel */
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

/* Vectors 32 to 256 are configurable by us */

/* System call interrupt vector */
#define SYSCALL 128 /* 0x80 */
extern void isr128();

/* Since we remap IRQs to interrupt vectors >= 32, assign a useful name to
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

/* Programmable IRQ vectors. These get remapped to vector 32 and onwards */
/* Master PIC */
extern void irq0(); /* system timer */
extern void irq1(); /* keyboard */
extern void irq2(); /* redirected to IRQ9 */
extern void irq3(); /* com 2/4 */
extern void irq4(); /* com 1/3 */
extern void irq5(); /* sound card */
extern void irq6(); /* floppy disk */
extern void irq7(); /* parallel */

/* Slave PIC */
extern void irq8(); /* RT clock */
extern void irq9(); /* redirected from IRQ2 */
extern void irq10(); /* reserved */
extern void irq11(); /* reserved */
extern void irq12(); /* ps2 mouse */
extern void irq13(); /* maths co-processor */
extern void irq14(); /* hard disk */
extern void irq15(); /* reserved */

/* phew. now look in interrupt.s... */

#endif
