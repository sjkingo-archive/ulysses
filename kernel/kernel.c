
#include "kernel.h"
#include "proc.h"

void kmain(void *mdb, unsigned int magic)
{
    unsigned char *vid = (unsigned char *)VIDMEM;
    vid[0] = 65; /* 'A' */
    vid[1] = 0x07; /* white on black */

    /* Set up the process table and scheduling queues */
    init_proc();
    
    /* Add IDLE as first proc in table and schedule it */
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* ignore the auto-generated pid */
    sched(get_proc(PID_IDLE));
}

