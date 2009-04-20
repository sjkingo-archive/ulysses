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

unsigned int initial_esp;
extern flag_t sched_active; /* sched.c */

/* idle_cpu()
 *  Loop forever in S1 sleep state. Set interrupts before halting the CPU
 *  to ensure the CPU has active interrupt lines.
 */
static void idle_cpu(void)
{
    TRACE_ONCE;
    while(1) __asm__ __volatile__("sti ; hlt");
}

void startup_kernel(void)
{
    TRACE_ONCE;

    /* This can only be run once: ensure we don't get here again */
    static flag_t already_started = FALSE;
    if (already_started) panic("startup_kernel() already set up");
    
    init_vt();
    print_startup();
    init_sched();
    init_task();
    init_initrd(*(unsigned int *)kern.mbi->mods_addr); /* set up root fs */
    init_shell("ulysses> "); /* this happens regardless of KERN_SHELL's val */

    already_started = TRUE;
    kprintf("Kernel startup complete in %ds\n", 
            kern.current_time_offset.tv_sec);

#if KERN_SHELL
    run_shell_all(); /* display shell */
#endif
}

void _kmain(void *mdb, unsigned int magic, unsigned int initial_stack)
{
    TRACE_ONCE;

    initial_esp = initial_stack; /* as given to us by multiboot */
    
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif
    
    /* Before we do anything with the higher-level kernel, move the kernel 
     * stack to a known location. This has to copy and remap all absolute
     * memory addresses, so can be quite slow if left until later.
     */
    move_stack((void *)STACK_LOC, STACK_SIZE);

#if KERN_INTERACTIVE
    print_startup();
    kprintf("Compiled with KERN_INTERATIVE, halting startup\n");
    init_shell("ulysses> ");
    run_shell();
#else
    startup_kernel();
#endif
 
    /* This is the kernel task (pid 0), so drop to an idle */
    sched_active = 1;
    idle_cpu();

    /* we should never get here */
    panic("kernel task died");
    return;
}
