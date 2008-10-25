
#include "kernel.h"
#include "proc.h"

static void print_startup(void)
{
    kprintf("Ulysses\tv0.2 (codename: Psycho)\n");
#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif
}

void _kmain(void *mdb, unsigned int magic)
{
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif
    
    /* Set up the virtual terminals - this will switch us to the LOG VT */
    if (!init_vt()) panic("Virtual terminals failed initialisation");
    print_startup();

    init_proc(); /* set up process table and scheduling queues */
    sanity_check(); /* do a sanity check of the kernel */

    /* Force IDLE going and never return */
    idle_task(); /* XXX */
    panic("_kmain() exited?");
}

