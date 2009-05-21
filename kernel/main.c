/* main.c - the kernel's entry point
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
#include "../arch/x86/x86.h" 

#include <sys/types.h>

/* This is the stack pointer that multiboot sets up for us. Eventually
 * we will move this to a higher address.
 */
unsigned int initial_esp;

/* idle_cpu()
 *  Loop forever in S1 sleep state. Set interrupts before halting the CPU
 *  to ensure the CPU has active interrupt lines.
 */
static void idle_cpu(void)
{
    TRACE_ONCE;
    while(1) __asm__ __volatile__("sti ; hlt");
}

void _kmain(void *mdb, unsigned int magic, unsigned int initial_stack)
{
    TRACE_ONCE;

    initial_esp = initial_stack; /* as given to us by multiboot */
 
    /* Perform architecture-specific startup stuff */
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif
    
    /* Before we do anything with the higher-level kernel, move the kernel 
     * stack to a known location. This has to copy and remap all absolute
     * memory addresses.
     */
    move_stack((void *)STACK_LOC, STACK_SIZE);
    
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

    /* And we're done. */
    kprintf("Kernel startup complete in %ds\n", 
            kern.current_time_offset.tv_sec);

    /* This is the kernel task (pid 0), so drop to an idle */
    idle_cpu();

    /* we should never get here */
    panic("kernel task died");
    return; /* and if we ever get here, then we really don't care anymore */
}
