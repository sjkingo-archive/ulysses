
#include "timer.h"
#include "isr/isr.h"
#include "util.h"

static void timer_callback(registers_t regs)
{
    (void)(regs); /* to keep the compiler happy */
    timer_tick(++ticks);
}

void init_timer(unsigned int freq)
{
    ticks = 0;
    register_interrupt_handler(IRQ0, &timer_callback);

    /* Initialise the timer */
    unsigned int div = CLOCK / freq;
    outb(0x43, 0x36); /* command byte */
    outb(0x40, (unsigned char)(div & 0xFF));
    outb(0x40, (unsigned char)((div >> 8) & 0xFF));
}

