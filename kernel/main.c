#include <ulysses/initrd.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/kthread.h>
#include <ulysses/paging.h>
#include <ulysses/sched.h>
#include <ulysses/shell.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/util.h>
#include <ulysses/vt.h>

#include "../arch/x86/x86.h" 

unsigned int initial_esp;

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
    move_stack((void*)0xE0000000, 0x2000);
    init_sched();
    init_task(); /* set up task management */
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
    initial_esp = initial_stack;

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

    return; /* we should never get here */
}
