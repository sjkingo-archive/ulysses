
#include "../../../kernel/kernel.h"
#include "isr.h"

void isr_handler(registers_t regs)
{
    kprintf("interrupt: %d\n", regs.int_no);
}

