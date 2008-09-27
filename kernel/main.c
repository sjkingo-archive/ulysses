
#include "kernel.h"
#include "proc.h"

void print_startup(void)
{
    kprintf("KOS - Kingo Operating System\n");
    kprintf("version 0.1 (if-it-works-have-a-party)\n\n");
    kprintf("XXX BIOS memory map\n");
}

void _kmain(void *mdb, unsigned int magic)
{
    /* Do initialisations */
    init_screen(); /* must be first */
    print_startup();

    /* Set up the process table and scheduling queues and add IDLE as first
     * proc in table.
     */
    init_proc();
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */

    /* Start IDLE going and never return */
    sched(get_proc(PID_IDLE));
    panic("_kmain() exited?");
}

