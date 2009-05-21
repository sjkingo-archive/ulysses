/* arch/x86/halt.c - CPU halt routine
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
 *
 *
 * This is the x86-specific halt code. First we try to disable interrupts
 * and halt the CPU. Fall back to a busy loop if that didn't work.
 */

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/util.h>

void halt(void)
{
    CLI; /* since we're about to die anyway */
    kprintf("\nKernel uptime %d seconds\n", kern.current_time_offset.tv_sec);
    kprintf("CPU halt\n");
    while (1) __asm__ __volatile__("hlt");

    /* Wow, the CPU on this machine must really be stuffed. Enter a busy
     * loop instead.
     */
    kprintf("\nCPU woken from halt; You must be Ken Thompson...\n");
    kprintf("\"I mean, if 10 years from now, when you are doing something "
            "quick and dirty,\nyou suddenly visualize that I am looking over "
            "your shoulders and say to\nyourself \'Dijkstra would not have "
            "liked this\', well, that would be enough\nimmortality for "
            "me.\"\n");
    kprintf("\t-- Edsger W. Dijkstra, 30 August 1995\n");
    while (1);
}
