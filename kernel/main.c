
#include "kernel.h"
#include "proc.h"

void _kmain(void *mdb, unsigned int magic)
{
    /* Set up the process table and scheduling queues */
    init_proc();
    
    /* Add IDLE as first proc in table and schedule it */
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */
    sched(get_proc(PID_IDLE));

    /* XXX just for testing */
    panic("Got milk?");
}

