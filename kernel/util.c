/* kernel/util.c - miscellaneous kernel functions
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
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/multiboot.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include "../config.h"

/* just kidding Dijkstra */
#define COMEFROM goto
void sanity_check(void)
{
    TRACE_ONCE;

    /* ... maybe not */
#ifdef COMEFROM
    COMEFROM pass; /* default to passing */
    COMEFROM fail; /* to keep the compiler happy */
    fail:
        panic("Kernel sanity check failed");
    pass:
#endif
        return; /* so we don't have an empty label */
}

void print_startup(void)
{
    TRACE_ONCE;
    kprintf("Ulysses\tv%s (codename: %s)\n", VERSION_NUM, VERSION_CN);
    kprintf("Kernel command line: '%s'\n", kern.cmdline);

#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif

#ifdef __GNUC__
    kprintf("Compiled by gcc %s on %s at %s (localtime)\n", __VERSION__, 
            __DATE__, __TIME__);
#else
    kprintf("Warning: not compiled by gcc; this disables a lot of nice "
            "debugging symbols\n");
#endif

#ifdef __LP64__
    kprintf("Compiled with 64-bit long int support\n");
#endif
#ifdef __STRICT_ANSI__
    kprintf("Warning: compiled with -ansi (how did that even work?)\n");
#endif
#ifdef __OPTIMIZE__
    kprintf("Warning: compiled with -Ox optimisations; expect limited "
            "debugging capacity\n");
#endif

#if PREEMPT_KERNEL
    kprintf("Compiled with PREEMPT_KERNEL; preempting kernel tasks\n");
#else
    kprintf("Warning: PREEMPT_KERNEL not enabled; kernel tasks must yield\n");
#endif

    kprintf("Detected %u KB of lower and %u KB of upper memory\n", 
            kern.mbi->mem_lower, kern.mbi->mem_upper);
    kprintf("Detected 1 CPU(s): %s %s\n", kern.cpu_vendor, kern.cpu_model);
    kprintf("CMOS-provided time 2%03d-%02d-%02d %02d:%02d:%02d (no timezone; "
            "probably UTC)\n", 
            kern.startup_datetime.year, kern.startup_datetime.month,
            kern.startup_datetime.day, kern.startup_datetime.hour,
            kern.startup_datetime.min, kern.startup_datetime.sec);
    kprintf("Kernel heap located at %p\n", KHEAP_START);
    kprintf("Kernel stack located at %p with size %d bytes\n", STACK_LOC, 
            STACK_SIZE);
}
