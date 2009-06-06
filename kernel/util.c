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

#include "../config.h"
#include <ulysses/cputest.h>
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

/* check_init(void)
 *  Check to make sure init is running, and restart it if not.
 */
static void check_init(void)
{
    static unsigned short i = 0;
    task_t *init;

    /* Don't just keep spinning and restarting init if it keeps dieing */
    if (i == 4) {
        panic("Giving up trying to restart init");
    }

    /* Get init's task and make sure it is valid */
    init = get_task(kern.init_pid);
    if (init == NULL || strcmp(init->name, "init") != 0) {
        kprintf("sanity_check(): init not running, trying to (re)start it\n");
        start_init();
        i++;
    }
}

/* run_init()
 *  init "task". Currently does nothing.
 */
static void run_init(void)
{
    TRACE_ONCE;
    kern.init_pid = do_getpid();
    change_name("init");
    kprintf("init(): running with pid %d\n", kern.init_pid);
    while (1);
}

void start_init(void)
{
    TRACE_ONCE;
    pid_t pid = do_fork();
    if (pid == 0) {
        run_init();
        task_exit();
    }
}

/* just kidding Dijkstra */
#define COMEFROM goto
void sanity_check(void)
{
    TRACE_ONCE;

    check_init();

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
    kprintf("Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>\n");
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

#if LOG_COM1
    kprintf("Sending all log messages to serial device COM1\n");
#endif

    kprintf("Detected %u KB of lower and %u KB of upper memory\n", 
            kern.mbi->mem_lower, kern.mbi->mem_upper);
    kprintf("Detected 1 CPU(s): %s %s at %d MHz\n", kern.cpu_vendor, 
            kern.cpu_model, kern.cpu_freq);
    test_cpu_bugs(); /* test for some known Intel bugs */
    kprintf("CMOS-provided time 2%03d-%02d-%02d %02d:%02d:%02d (no timezone; "
            "probably UTC)\n", 
            kern.startup_datetime.year, kern.startup_datetime.month,
            kern.startup_datetime.day, kern.startup_datetime.hour,
            kern.startup_datetime.min, kern.startup_datetime.sec);
    kprintf("Kernel heap located at %p\n", KHEAP_START);
    kprintf("Kernel stack located at %p with size %d bytes\n", STACK_LOC, 
            STACK_SIZE);
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
