
#ifndef _PROC_H
#define _PROC_H

#include <types.h>

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

/* An entry in the process table. When updating this, be sure to keep 
 * init_proc() updated as well.
 */
struct proc {
    pid_t pid; /* process id */
    uid_t uid; /* user id */
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    unsigned short sched_q; /* scheduling queue this proc belongs to */
    flag_t ready; /* whether this process is ready to be run */

    unsigned short s_ticks_left; /* number of scheduling ticks left */
    unsigned short s_quantum_size; /* quantum size in ticks */
};

#define NR_PROCS 100 /* max processes we can handle */
struct proc proc[NR_PROCS]; /* the process table itself */
#define BEG_PROC (&proc[0]) /* pointer to the start of proc table */
#define END_PROC (&proc[NR_PROCS]) /* pointer to the end of the proc table */ 

/* Core kernel process pids */
#define PID_NONE -1
#define PID_IDLE 0

/* Scheduling */
#define SCHED_Q_NR 3 /* number of scheduling queues to use (IDLE_Q + 1) */
#define IDLE_Q 2 /* lowest queue, IDLE process always stays here */
#define USER_Q 1 /* user processes go here */
#define ROOT_Q 0 /* root's processes go here so they are picked first */
#define SCHED_QUANTUM 50 /* default quantum for processes */
struct proc *next_proc; /* next process to run */

void init_proc(void);
struct proc *get_proc(pid_t pid);

#endif

