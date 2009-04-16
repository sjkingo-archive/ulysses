
#include <ulysses/timer.h>
#include <ulysses/kernel.h>

void timer_tick(registers_t regs)
{
    if (regs.int_no != IRQ0) panic("timer_tick() called for wrong IRQ!");
    ticks++;

    /* Increment every second */
    if ((ticks % TIMER_FREQ) == 0) {
        kern.current_time_offset.tv_sec++;
    }

    sanity_check();
    check_current_task();
}

void init_timer(unsigned int freq)
{
    ticks = 0;
    register_interrupt_handler(IRQ0, &timer_tick);

    /* Initialise the timer */
    unsigned int div = CLOCK / freq;
    outb(0x43, 0x36); /* command byte */
    outb(0x40, (unsigned char)(div & 0xFF));
    outb(0x40, (unsigned char)((div >> 8) & 0xFF));
}

