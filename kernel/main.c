
#include "kernel.h"
#include "proc.h"

void _kmain(void *mdb, unsigned int magic)
{
    /* Initialise screen - this must be the very first thing we do! */
    init_screen();

    /* Print some pretty startup info */
    kprintf("KOS - Kingo Operating System\n");
    kprintf("version 0.1 (if-it-works-have-a-party)\n\n");
    kprintf("XXX BIOS memory map\n");

    /* Set up the process table and scheduling queues */
    init_proc();
    
    /* Add IDLE as first proc in table and schedule it */
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */
    sched(get_proc(PID_IDLE));

    panic("_kmain() exited?");
}

