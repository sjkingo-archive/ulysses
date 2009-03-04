
#ifndef _IDT_H
#define _IDT_H

/* Interrupt descriptor table. We load this into the CPU so it can determine
 * interrupt vectors and exception handling responses.
 *
 * Interrupt handlers do not belong here: see isr.{c,h}.
 */

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
void init_idt(void);

/* See flush.s for assembler routine for this */
/* XXX move this to inline assembler */
extern void idt_flush(unsigned int);

#endif

