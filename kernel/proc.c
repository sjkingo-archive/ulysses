
#include "kernel.h"

#include "proc.h"

#include <string.h>

static pid_t new_pid(void)
{
    return PID_NONE; /* XXX */
}

void init_proc(void)
{
    register struct proc *p;
    register unsigned short i;
    static flag_t initProc;

    /* Only allow this function to be called once */
    if (initProc) panic("init_proc() already set up");
    initProc = 1;
    
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

    /* Set up the idle process */
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */
    sched(get_proc(PID_IDLE));
#if DEBUG
    kprintf("init_proc(): IDLE process set up\n");
#endif
}

void new_proc(uid_t uid, gid_t egid, gid_t rgid, char *name)
{
    unsigned int index = ++last_proc; /* get this proc's index */
    proc[index].pid = new_pid(); /* generate a new pid */
    proc[index].uid = uid;
    proc[index].egid = egid;
    proc[index].rgid = rgid;
    strcpy(proc[index].name, name);
    proc[index].ready = 1; /* ready to run */
    proc[0].s_ticks_left = SCHED_QUANTUM;
    proc[0].s_quantum_size = SCHED_QUANTUM;

#if SCHED_DEBUG
    kprintf("new_proc(): %s, pid %d, uid %d, egid %d, rgid %d\n", 
            proc[index].name, (unsigned int)proc[index].pid, 
            (unsigned int)proc[index].uid, (unsigned int)proc[index].egid,
            (unsigned int)proc[index].rgid);
#endif
}

struct proc *get_proc(pid_t pid)
{
    register struct proc *p;
    for (p = BEG_PROC; p < END_PROC; ++p) {
        if (p->pid == pid) return p;
    }
    return NULL;
}

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

#if SCHED_DEBUG
    kprintf("sched(): pid %d added to queue %d\n", p->pid, p->sched_q);
#endif
}

void pick_proc(void)
{
    register unsigned short i;
    register struct proc *p;

    for (i = 0; i < NR_SCHED_Q; i++) {
        p = sched_queues[i].head; /* head of the queue */
        while (p != NULL) {
            if (p->ready) { /* ready process found, schedule it to be run */
                next_proc = p;
                curr_pid = p->pid;
#if SCHED_DEBUG
                kprintf("pick_proc(): %s, pid %d\n", p->name, 
                        (unsigned int)p->pid);
#endif
                return;
            }
            p = p->next;
        }
    }
}

void print_current_proc(void)
{
    register struct proc *p = get_proc(curr_pid);
    kprintf("Current process information:\n");
    kprintf("\tpid: %d\t%s\tsched_q: %d\n", p->pid, p->name, p->sched_q);
    kprintf("\tuid: %d\tegid: %d\trgid: %d\n", p->uid, p->egid, p->rgid);
}

void check_current_proc(void)
{
    register struct proc *p = get_proc(curr_pid);
    if (--p->s_ticks_left <= 0) {
#if SCHED_DEBUG
        kprintf("check_current_proc(): pid %d (%s) exceeded scheduling "
                "quanta\n", p->pid, p->name);
#endif
        p->s_ticks_left = p->s_quantum_size;
        pick_proc();
    }
}

