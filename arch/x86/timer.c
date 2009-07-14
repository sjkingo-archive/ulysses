/* arch/x86/timer.c - clock timer
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/timer.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

static unsigned long ticks;
static unsigned int tick_interval;

void timer_tick(registers_t *regs)
{
    TRACE_ONCE;
    if (regs->int_no != IRQ0) panic("timer_tick() called for wrong IRQ!");
    
    /* A timer tick occurs every x milliseconds, so add that interval to the
     * kernel's msec counter and ask the task manager to update the CPU time
     * of the current task.
     */
    ticks++;
    kern.current_time_offset.tv_msec += tick_interval;
    update_cpu_time();

    /* Update the kernel's second counter every second (1000 ms) */
    if ((kern.current_time_offset.tv_msec % 1000) == 0) {
        kern.current_time_offset.tv_sec++;
        sanity_check();

        if (kern.flags.debug_ticks) {
            kprintf("timer_tick(): %lds (%ldms) has passed since PIT init\n", 
                    kern.current_time_offset.tv_sec,
                    kern.current_time_offset.tv_msec);
        }
    }

    check_current_task();
}

void init_timer(unsigned int freq)
{
    TRACE_ONCE;
    ticks = 0;
    tick_interval = 1000 / TIMER_FREQ; /* timer ticks every x msecs */

    /* Initialise the timer */
    unsigned int div = CLOCK / freq;
    outb(0x43, 0x36); /* command byte */
    outb(0x40, (unsigned char)(div & 0xFF));
    outb(0x40, (unsigned char)((div >> 8) & 0xFF));
}

unsigned long get_ticks(void)
{
    return ticks;
}
