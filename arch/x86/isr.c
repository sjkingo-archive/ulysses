
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

void test_interrupts(void)
{
    __asm__("int $0x00");
    __asm__("int $0x01");
    __asm__("int $0x02");
    __asm__("int $0x03");
    __asm__("int $0x04");
    __asm__("int $0x05");
    __asm__("int $0x06");
    __asm__("int $0x07");
    __asm__("int $0x08");
    __asm__("int $0x09");
    __asm__("int $0x10");
    __asm__("int $0x11");
    __asm__("int $0x12");
    __asm__("int $0x13");
    __asm__("int $0x14");
    __asm__("int $0x15");
    __asm__("int $0x16");
    __asm__("int $0x17");
    __asm__("int $0x18");
}

void register_interrupt_handler(unsigned char n, isr_t handler)
{
    interrupt_handlers[n] = handler;
#if DEBUG
    kprintf("Interrupt handler registered for INT %d\n", n);
#endif
}

void irq_handler(registers_t regs)
{
    /* Write an end of interrupt signal to the PICs */
    if (regs.int_no >= IRQ0) {
        if (regs.int_no >= IRQ8) outb(0xA0, 0x20); /* slave */
        outb(0x20, 0x20); /* master */
    }

    /* Since we don't care about a lot of IRQs, we can just ignore any
     * that don't have a handler. Do this here instead of calling the
     * interrupt handler above (which would panic if unhandled).
     */
    isr_t handler = interrupt_handlers[regs.int_no];
    if (handler == NULL) kprintf("Unhandled IRQ %d\n", regs.int_no);
    else handler(regs);
}

