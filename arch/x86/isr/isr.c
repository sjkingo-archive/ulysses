
#include "../../../kernel/kernel.h"
#include "isr.h"

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

    /* Make sure you never call return from inside the switch - this will
     * leave interrupts *disabled*!!
     */
    switch (regs.int_no) {
        case 0: /* div by zero */
            kprintf("Recovering from a div by zero; needs better maths...\n");
            break;

        case 3: /* breakpoint */
            kprintf("Hardware breakpoint\n");
            break;

        default:
            panic("Unhandled interrupt");
    }

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

