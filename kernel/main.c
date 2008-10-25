
#include "kernel.h"
#include "proc.h"

static void print_startup(void)
{
    kprintf("Ulysses\tv0.2 (codename: Psycho)\n");
#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif
}

void sanity_check(void)
{
    stub((char *)__func__);

    goto pass; /* default to passing */
    goto fail; /* to keep the compiler happy */
    fail:
        panic("Kernel sanity check failed");
    pass:
#if DEBUG
        kprintf("Kernel sanity check passed\n");
#endif
        return; /* so we don't have an empty label with DEBUG 0 */
}

void stub(char *fname)
{
#if DEBUG
    kprintf("stub: %s()\n", fname);
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

