
#include "proc.h"
#include "sched.h"

#include <unistd.h>

void init_proc(void)
{
    register struct proc *p;
    
    /* Blank the process table */
    for (p = BEG_PROC; p < END_PROC; ++p) {
        p->pid = PID_NONE; /* -1 signifies nothing */
    }

    /* Add IDLE as first proc in table */
    proc[0].pid = 0;
    proc[0].uid = -1;
    proc[0].egid = -1;
    proc[0].rgid = -1;
    proc[0].sched_q = IDLE_Q;
    proc[0].s_ticks_left = SCHED_QUANTUM;
    proc[0].s_quantum_size = SCHED_QUANTUM;
}

struct proc *get_proc(pid_t pid)
{
    register struct proc *p;
    for (p = BEG_PROC; p < END_PROC; ++p) {
        if (p->pid == pid) return p;
    }
    return NULL;
}

