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

#include <ulysses/cpu.h>
#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/multiboot.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <string.h>

/* Time to do low-level setup */
extern unsigned long long startup_time;

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

    kprintf("%[0,15]");
    kprintf("Ulysses\tv%s (codename: %s)\n", VERSION_NUM, VERSION_CN);
    kprintf("Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>\n\n");
    kprintf("%[0,7]");

    kprintf("Low-level startup complete in %Ldms\n", startup_time);
    kprintf("Kernel command line: '%s'\n", kern.cmdline);
    kprintf("Sending all log messages to serial device COM1\n");

#ifdef __GNUC__
    kprintf("Compiled by gcc %s on %s at %s (localtime)\n", __VERSION__, 
            __DATE__, __TIME__);
#else
    kprintf("Warning: not compiled by gcc; this disables a lot of nice "
            "debugging symbols\n");
#endif

    print_memory_map();
    kprintf("Detected 1 CPU(s): %s %s at %d MHz\n", kern.cpu_vendor, 
            kern.cpu_model, kern.cpu_freq);
    kprintf("CMOS-provided time 2%0*d-%0*d-%0*d %0*d:%0*d:%0*d (no timezone; "
            "probably UTC)\n", 3, kern.startup_datetime.year, 
            2, kern.startup_datetime.month,
            2, kern.startup_datetime.day,
            2, kern.startup_datetime.hour,
            2, kern.startup_datetime.min, 
            2, kern.startup_datetime.sec);
}

inline void lock_kernel(void)
{
    TRACE_ONCE;
    CLI;
}

inline void unlock_kernel(void)
{
    TRACE_ONCE;
    STI;
}

void parse_cmdline(const char *cmdline)
{
    char *s;
    while ((s = strsep(&cmdline, " ")) != NULL) {
        if (strcmp(s, "--no-preempt-kernel") == 0) {
            kern.flags.preempt_kernel = FALSE;
        } else if (strcmp(s, "--debug-sched") == 0) {
            kern.flags.debug_sched = TRUE;
        } else if (strcmp(s, "--debug-task") == 0) {
            kern.flags.debug_task = TRUE;
        } else if (strcmp(s, "--debug-interrupt") == 0) {
            kern.flags.debug_interrupt = TRUE;
        } else if (strcmp(s, "--debug-ticks") == 0) {
            kern.flags.debug_ticks = TRUE;
        }
    }
}
