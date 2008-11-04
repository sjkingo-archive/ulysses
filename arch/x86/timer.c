
#include "x86.h"

void timer_tick(registers_t regs)
{
    ticks++;
}

flag_t init_timer(unsigned int freq)
{
    ticks = 0;
    register_interrupt_handler(IRQ0, &timer_tick);

    /* Initialise the timer */
    unsigned int div = CLOCK / freq;
    outb(0x43, 0x36); /* command byte */
    outb(0x40, (unsigned char)(div & 0xFF));
    outb(0x40, (unsigned char)((div >> 8) & 0xFF));

    kprintf("Timer at %d Hz (divisor %d)\n", freq, div);

    return TRUE; /* ensure a sane return for _kmain() */
}

