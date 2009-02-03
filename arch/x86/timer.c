
#include "x86.h"
#include "../../kernel/kernel.h"

void timer_tick(registers_t regs)
{
    if (regs.int_no != IRQ0) panic("timer_tick() called for wrong IRQ!");
    ticks++;

    /* Increment every second */
    if ((ticks % TIMER_FREQ) == 0) {
        kern.current_time_offset.tv_sec++;
    }
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

