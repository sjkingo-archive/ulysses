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

/* flag to tell us whether to try and restart init or not */
flag_t restart_init = FALSE;

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
        i++;
        kprintf("sanity_check(): init not running, trying to (re)start it\n");
        start_init();
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
    kprintf("init: running with pid %d\n", kern.init_pid);
    idle_cpu();
}

void start_init(void)
{
    TRACE_ONCE;
    pid_t pid = do_fork();
    if (pid == 0) {
        run_init();
        task_exit();
    } else if (pid == -1) {
        kprintf("fork() returned -1 in pid %d\n", do_getpid());
    }
}

/* just kidding Dijkstra */
#define COMEFROM goto
void sanity_check(void)
{
    TRACE_ONCE;

    if (restart_init) {
        check_init();
    }

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
