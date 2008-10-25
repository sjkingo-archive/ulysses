
#include "../x86.h"

void isr_handler(registers_t regs)
{
    /* We need to disable interrupts as quickly as possible in case another
     * is generated: this prevents an interrupt storm.
     */
    __asm__ __volatile__("cli");

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

    /* If we are returning to kernel proper, re-enable interrupts */
    __asm__ __volatile__("sti");
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
    kprintf("Interrupt handler registered for IRQ %d\n", n);
#endif
}

void irq_handler(registers_t regs)
{
    __asm__ __volatile("cli");

    /* Write an end of interrupt signal to the PICs */
    if (regs.int_no >= 40) outb(0xA0, 0x20); /* if source, send to slave PIC */
    outb(0x20, 0x20); /* always send to master PIC */

    /* Call the handler for this IRQ */
    if (interrupt_handlers[regs.int_no] == 0) return;
    
    isr_t handler = interrupt_handlers[regs.int_no];
    __asm__ __volatile__("sti"); /* enable interrupts before jumping */
    handler(regs); /* jump to the handler */
}

