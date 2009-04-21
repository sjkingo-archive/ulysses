#include <ulysses/isr.h>
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

void isr_handler(registers_t regs)
{
    TRACE_ONCE;
    symbol_t *sym;
    void *addr = __builtin_return_address(1);

    kprintf("\nInterrupt %d at %p, inside %p ", regs.int_no, regs.eip, addr);
    
    /* Try to translate the last func's eip into a symbol */
    sym = lookup_symbol(addr);
    if (sym == NULL) kprintf("in ??? ()\n");
    else kprintf("in %s ()\n", sym->name);

    kprintf("err_code %d; dumping registers:\n", regs.err_code);
    kprintf("edi %p\tesi %p\tebp %p\tesp %p\n"
            "ebx %p\tedx %p\tecx %p\teax %p\n", regs.edi, regs.esi, 
            regs.ebp, regs.esp, regs.ebx, regs.edx, regs.ecx, regs.eax);

    /* If a handler exists, call it */
    isr_t handler = interrupt_handlers[regs.int_no];
    if (handler == NULL) panic("Unhandled interrupt");
    handler(regs);
}

void register_interrupt_handler(unsigned char n, isr_t handler)
{
    TRACE_ONCE;
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t regs)
{
    TRACE_ONCE;
    /* Since we don't care about a lot of IRQs, we can just ignore any
     * that don't have a handler.
     */
    isr_t handler = interrupt_handlers[regs.int_no];
    if (handler != NULL) handler(regs);

    /* Write an end of interrupt signal to the PICs */
    if (regs.int_no >= IRQ0) {
        if (regs.int_no >= IRQ8) outb(0xA0, 0x20); /* slave */
        outb(0x20, 0x20); /* master */
    }
}
