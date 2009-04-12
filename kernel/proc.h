
#ifndef _PROC_H
#define _PROC_H

/* Here is the process table and the kernel scheduler.
 *
 * The scheduler is currently a queue-based, round-robin scheduler. It does 
 * *not* take uid into account. Queue 0 is the highest priority, meaning
 * whichever process resides at the head of queue 0 is next to be executed on
 * the CPU.
 *
 * The lowest priority queue contains IDLE. If no processes are ready
 * in the higher queues, we fall back to the idle process and execute it.
 * IDLE *must* stay as the only process in the lowest priority queue!
 */

#include "../arch/x86/paging.h"

#define NR_SCHED_Q 2 /* number of scheduling queues to use */

/* Core kernel process pids */
#define PID_NONE -1

/* An entry in the process table. When updating this, be sure to keep 
 * new_proc() updated as well.
 */
typedef struct proc {
    pid_t pid; /* process id */
    uid_t uid; /* user id */
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    char *name; /* NULL-terminated process name */

    unsigned int esp; /* current stack pointer */
    unsigned int ebp; /* base stack pointer */
    unsigned int eip; /* current instruction pointer */
    page_dir_t *page_dir;

    unsigned short sched_q; /* scheduling queue this proc belongs to */
    flag_t ready; /* whether this process is ready to be run */
    unsigned short s_ticks_left; /* number of scheduling ticks left */
    unsigned short s_quantum_size; /* quantum size in ticks */

    struct proc *next; /* next proc in queue, or NULL if tail */
} proc_t;

#define USER_Q 1 /* user processes go here */
#define ROOT_Q 0 /* root's processes go here so they are picked first */
#define SCHED_QUANTUM 50 /* default quantum for processes */

/* A scheduling queue. Be sure to update init_sched() when changing this. */
struct sched_q {
    unsigned short q_num; /* number of this queue */
    struct proc *head; /* pointer to head process on queue, NULL if empty */
    struct proc *tail; /* pointer to tail process on queue, NULL if empty */
};

struct sched_q sched_queues[NR_SCHED_Q]; /* all of the scheduling queues */

/* init_proc()
 *  Initialises the process table and scheduling queues. This needs to be
 *  called *once* and once only. A panic() will be triggered if this is
 *  called again.
 */
void init_proc(void);

pid_t fork(void);

pid_t getpid(void);

void check_current_proc(void);

#endif

