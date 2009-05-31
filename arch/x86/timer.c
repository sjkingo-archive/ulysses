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
#include <ulysses/timer.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

static unsigned long ticks;

void timer_tick(registers_t regs)
{
    TRACE_ONCE;
    if (regs.int_no != IRQ0) panic("timer_tick() called for wrong IRQ!");
    ticks++;

    /* Increment every second */
    if ((ticks % TIMER_FREQ) == 0) {
        kern.current_time_offset.tv_sec++;
        update_cpu_time();
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

unsigned long get_ticks(void)
{
    return ticks;
}
