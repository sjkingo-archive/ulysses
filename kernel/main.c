
#include "kernel.h"
#include "proc.h"

/* Internal function prototypes */
void print_startup(void);

void print_startup(void)
{
    kprintf("KOS - Kingo Operating System\n");
    kprintf("v0.1\t(Codename: Nomad)\n\n");
}

void sanity_check(void)
{
    if (kern.mdb == NULL) panic("(sanity) kern.mdb is NULL");
    if (kern.magic <= 0) panic("(sanity) kern.magic <= 0");

#if DEBUG
    kprintf("Kernel sanity check passed\n");
#endif
}

void _kmain(void *mdb, unsigned int magic)
{
    init_screen(); /* must be first in _kmain() */

    /* Set up the kernel data structure */
    kern.mdb = mdb;
    kern.magic = magic;

    /* Print startup messages */
    print_startup();
    kprintf("Multiboot magic: %d\n", kern.magic);
    /* XXX BIOS memory map */

    /* Set up the process table and scheduling queues and add IDLE as first
     * proc in table.
     */
    init_proc();
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */

    sanity_check(); /* do a sanity check of the kernel */

    /* Start IDLE going and never return */
    sched(get_proc(PID_IDLE));
    panic("_kmain() exited?");
}

