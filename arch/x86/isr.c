
#include "x86.h"

void isr_handler(registers_t regs)
{
#if DEBUG
    kprintf("\nInterrupt: %d, err_code %d; dumping registers:\n", 
            regs.int_no, regs.err_code);
    kprintf("\tedi %p\tesi %p\tebp %p\tesp %p\n\tebx %p\tedx %p"
            "\tecx %p\teax %p\n", regs.edi, regs.esi, regs.ebp, regs.esp,
            regs.ebx, regs.edx, regs.ecx, regs.eax);
#endif

    /* If a handler exists, call it */
    isr_t handler = interrupt_handlers[regs.int_no];
    if (handler == NULL) panic("Unhandled interrupt");
    handler(regs);
}

void register_interrupt_handler(unsigned char n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t regs)
{
    /* Write an end of interrupt signal to the PICs */
    if (regs.int_no >= IRQ0) {
        if (regs.int_no >= IRQ8) outb(0xA0, 0x20); /* slave */
        outb(0x20, 0x20); /* master */
    }

    /* Since we don't care about a lot of IRQs, we can just ignore any
     * that don't have a handler.
     */
    isr_t handler = interrupt_handlers[regs.int_no];
    if (handler != NULL) handler(regs);
#if DEBUG
    else kprintf("Unhandled IRQ %d\n", regs.int_no);
#endif
}

