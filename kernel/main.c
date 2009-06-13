/* kernel/main.c - the kernel's entry point
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

#include <ulysses/initrd.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/kthread.h>
#include <ulysses/paging.h>
#include <ulysses/sched.h>
#include <ulysses/shell.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>
#include <ulysses/vt.h>

#include "../config.h"

#include <sys/types.h>
#include <unistd.h>

struct kernel kern;

#ifdef _ARCH_x86
extern void startup_x86(void *, int);
#endif

/* see util.c */
extern flag_t restart_init;

/* This is the stack pointer that multiboot sets up for us. Eventually
 * we will move this to a higher address.
 */
unsigned int initial_esp;

void _kmain(void *mdb, unsigned int magic, unsigned int initial_stack)
{
    TRACE_ONCE;

    initial_esp = initial_stack; /* as given to us by multiboot */
    
    /* Set up the default kernel flags. Change these by adding their 
     * corresponding option when booting the kernel.
     */
    kern.flags.preempt_kernel = TRUE;
    kern.flags.debug_sched = FALSE;
    kern.flags.debug_task = FALSE;
    kern.flags.debug_interrupt = FALSE;
    kern.flags.debug_ticks = FALSE;
 
    /* Perform architecture-specific startup stuff */
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif

    /* Before we do anything with the higher-level kernel, move the kernel 
     * stack to a known location. This has to copy and remap all absolute
     * memory addresses.
     */
    move_stack((void *)STACK_LOC, STACK_SIZE);

    /* Extract and set any options given to the kernel */
    parse_cmdline(kern.cmdline);
    
    /* Now that all the lower-level startup has been done, we can set up
     * the higher-level kernel functions.
     */
    init_sched();
    init_task();
    init_vt();
    print_startup();

    /* Start the kthread daemon going to set up other kernel threads */
    kthread_create(kthreadd, "kthreadd");
    
    /* XXX this is where we would load init process off the initrd */
    start_init();
    restart_init = TRUE;

    /* And we're done */
    kprintf("Kernel startup complete in %ds (%dms)\n", 
            kern.current_time_offset.tv_sec,
            kern.current_time_offset.tv_msec);
    
    /* We need to wait a bit here for all the threads to settle themselves 
     * before going away.
     */
    sleep(100);

    /* The kernel task has finished the startup, so remove the task from the
     * system. This should never return, so panic if it does.
     */
    task_exit();
    panic("kernel task died");
    return; /* and if we ever get here, then we really don't care anymore */
}
