
#include "kernel.h"
#include "proc.h"

/* init_proc()
 *  Initialises the process table and scheduling queues.
 *  This needs to be called *once* by kmain(), and never again.
 */
void init_proc(void)
{
    register struct proc *p;
    register unsigned short i;
    
    /* Blank the process table */
    for (p = BEG_PROC; p < END_PROC; ++p) {
        p->pid = PID_NONE; /* -1 signifies nothing */
    }

    /* Blank the scheduling queues */
    for (i = 0; i < NR_SCHED_Q; i++) {
        sched_queues[i].q_num = i;
        sched_queues[i].head = NULL;
        sched_queues[i].tail = NULL;
    }

#if DEBUG
    kprintf("init_proc(): proc table set up\n");
#endif
}

/* new_pid()
 *  Generate a new pid and return it.
 */
pid_t new_pid(void)
{
    return PID_NONE; /* XXX */
}

/* new_proc()
 *  Add a new process to the process table. Note that this process will
 *  *not* be scheduled at this time. Call sched() for that.
 */
void new_proc(uid_t uid, gid_t egid, gid_t rgid, char *name)
{
    unsigned int index = ++last_proc; /* get this proc's index */
    proc[index].pid = new_pid(); /* generate a new pid */
    proc[index].uid = uid;
    proc[index].egid = egid;
    proc[index].rgid = rgid;
    /* XXX copy name */
    proc[index].ready = 1; /* ready to run */
    proc[0].s_ticks_left = SCHED_QUANTUM;
    proc[0].s_quantum_size = SCHED_QUANTUM;
}

/* get_proc()
 *  Searches the process table for a process with the given pid. Returns
 *  a pointer to the process struct if found, or NULL if not.
 */
struct proc *get_proc(pid_t pid)
{
    register struct proc *p;
    for (p = BEG_PROC; p < END_PROC; ++p) {
        if (p->pid == pid) return p;
    }
    return NULL;
}

/* sched()
 *  Determines which scheduling queue to add the given process to, and
 *  adds it.
 */
void sched(struct proc *p)
{
    register struct sched_q q;

    /* Pick the queue and update the process */
    if (p->uid == 0) {
        if (p->pid == PID_IDLE) p->sched_q = IDLE_Q;
        else p->sched_q = ROOT_Q;
    } else {
        p->sched_q = USER_Q;
    }
    q = sched_queues[p->sched_q];

    p->next = NULL;
    if (q.head == NULL) {
        /* Add to head of queue */
        q.head = p;
        q.tail = p;
    } else {
        /* Add to tail */
        q.tail->next = p;
        q.tail = p;
    }

#if DEBUG
    kprintf("sched(): pid %d added to queue %d\n", p->pid, p->sched_q);
#endif
}

/* pick_proc()
 *  Picks a process to be run next. This starts with the head of queue 0, and
 *  proceeds to the only process in the lowest queue - IDLE. Once a runnable
 *  process is found, point next_proc to it and return.
 */
void pick_proc(void)
{
    register unsigned short i;
    register struct proc *p;

    for (i = 0; i < NR_SCHED_Q; i++) {
        p = sched_queues[i].head; /* head of the queue */
        while (p != NULL) {
            if (p->ready) { /* ready process found, schedule it to be run */
                next_proc = p;
                return;
            }
            p = p->next;
        }
    }
}

