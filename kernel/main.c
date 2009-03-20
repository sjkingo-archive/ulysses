
#include "kernel.h"

/* idle_task()
 *  Loop forever in S1 sleep state. Set interrupts before halting the CPU
 *  to ensure the CPU has active interrupt lines.
 */
static void idle_task(void)
{
    while (1) __asm__ __volatile__("sti ; hlt");
}

void startup_kernel(void)
{
    /* This can only be run once: ensure we don't get here again */
    static flag_t already_started = FALSE;
    if (already_started) panic("startup_kernel() already set up");
    
    init_vt();
    print_startup();
#if DEBUG
    print_cpuinfo();
    print_meminfo();
#endif
    init_kthread();
    init_proc(); /* set up process table and scheduling queues */
    init_initrd(*(unsigned int *)kern.mbi->mods_addr); /* set up root fs */
    init_shell("ulysses> "); /* this happens regardless of KERN_SHELL's val */

    already_started = TRUE;
    kprintf("Kernel startup complete in %ds\n", 
            kern.current_time_offset.tv_sec);

#if KERN_SHELL
    run_shell(); /* display shell */
#endif
}

void _kmain(void *mdb, unsigned int magic)
{
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif

#if KERN_INTERACTIVE
    print_startup();
    kprintf("Compiled with KERN_INTERATIVE, halting startup\n");
    init_shell("ulysses> ");
    run_shell();
#else
    startup_kernel();
#endif

    /* Once startup is complete, the main kernel thread drops to an idle
     * loop, placing the CPU in a S1 sleep state.
     */
    idle_task();
    return; /* we should never get here */
}

