
#include "kernel.h"
#include "proc.h"

/* Internal function prototypes */
pid_t new_pid(void);

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
}

pid_t new_pid(void)
{
    return PID_NONE; /* XXX */
}

void new_proc(uid_t uid, gid_t egid, gid_t rgid, char *name)
{
    unsigned int index = ++last_proc; /* get this proc's index */
    proc[index].pid = new_pid(); /* generate a new pid */
    proc[index].uid = uid;
    proc[index].egid = egid;
    proc[index].rgid = rgid;
    proc[index].name[0] = name[0]; /* XXX to keep the compiler happy */
    proc[index].ready = 1; /* ready to run */
    proc[0].s_ticks_left = SCHED_QUANTUM;
    proc[0].s_quantum_size = SCHED_QUANTUM;

#if DEBUG
    kprintf("new_proc(): %s, pid %d, uid %d, egid %d, rgid %d\n", name,
            (unsigned int)uid, (unsigned int)egid, (unsigned int)rgid);
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

#if DEBUG
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
#if DEBUG
                kprintf("pick_proc(): %s, pid %d\n", p->name, 
                        (unsigned int)p->pid);
#endif
                return;
            }
            p = p->next;
        }
    }
}

void idle_task(void)
{
    kprintf("Putting CPU in idle...\n");
    __asm__("hlt");
}

