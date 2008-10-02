
#ifndef _IDT_H
#define _IDT_H

/* Interrupt descriptor table. This declares all of the CPU interrupts handling
 * code. Be sure to not corrupt the IDT, or the CPU will be unable to lookup
 * any interrupts and instead default to triple-faulting.
 */

#include <types.h>

struct idt_entry_struct {
    unsigned short base_low; /* lower 16 bits to jump to */
    unsigned short sel; /* segment selector */
    unsigned char always0; /* just as the name says... */
    unsigned char flags;
    unsigned short base_high; /* higher 16 bits to jump to */
} __attribute__((packed)); /* pack to 1 byte */
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)); /* pack to 1 byte */
typedef struct idt_ptr_struct idt_ptr_t;

idt_ptr_t idt_ptr;
idt_entry_t idt_entries[256]; /* that's a lot of interrupts */

/* init_idt()
 *  Initialise the interrupt descriptor table.
 */
flag_t init_idt(void);

/* See flush.s for assembler routine for this */
/* XXX move this to inline assembler */
extern void idt_flush(unsigned int);

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

