
#include "proc.h"

#include <unistd.h>

void init_proc(void)
{
    register struct proc *p;
    for (p = BEG_PROC; p < END_PROC; ++p) {
        p->pid = -1; /* -1 signifies nothing */
    }
}

struct proc *get_proc(pid_t pid)
{
    register struct proc *p;
    for (p = BEG_PROC; p < END_PROC; ++p) {
        if (p->pid == pid) return p;
    }
    return NULL;
}

