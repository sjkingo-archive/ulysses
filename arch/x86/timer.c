#include <ulysses/kernel.h>
#include <ulysses/timer.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

void timer_tick(registers_t regs)
{
    TRACE_ONCE;
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
    TRACE_ONCE;
    ticks = 0;

    /* Initialise the timer */
    unsigned int div = CLOCK / freq;
    outb(0x43, 0x36); /* command byte */
    outb(0x40, (unsigned char)(div & 0xFF));
    outb(0x40, (unsigned char)((div >> 8) & 0xFF));
}
