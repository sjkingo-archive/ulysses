
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

#define NR_PROCS 100 /* max processes we can handle */
#define NR_SCHED_Q 3 /* number of scheduling queues to use */

/* Core kernel process pids */
#define PID_NONE -1
#define PID_IDLE 0

/* An entry in the process table. When updating this, be sure to keep 
 * new_proc() updated as well.
 */
struct proc {
    pid_t pid; /* process id */
    uid_t uid; /* user id */
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    char *name; /* NULL-terminated process name */

    void (*task)(void); /* XXX until context switching is added, we just
                           have a pointer to a function as a "process" */

    unsigned short sched_q; /* scheduling queue this proc belongs to */
    flag_t ready; /* whether this process is ready to be run */
    unsigned short s_ticks_left; /* number of scheduling ticks left */
    unsigned short s_quantum_size; /* quantum size in ticks */
    struct proc *next; /* next proc in scheduling queue, or NULL if tail */
};

/* Process table */
struct proc proc[NR_PROCS]; /* the process table itself */
unsigned int last_proc; /* index of last proc in the table */
#define BEG_PROC (&proc[0]) /* pointer to the start of proc table */
#define END_PROC (&proc[NR_PROCS]) /* pointer to the end of the proc table */ 

#define IDLE_Q 2 /* lowest queue, IDLE process always stays here */
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
struct proc *next_proc; /* next process to run */

pid_t curr_pid; /* current pid running */

/* init_proc()
 *  Initialises the process table and scheduling queues. This needs to be
 *  called *once* and once only. A panic() will be triggered if this is
 *  called again.
 */
void init_proc(void);

/* new_proc()
 *  Add a new process to the process table. Note that this process will
 *  *not* be scheduled at this time. Call sched() for that.
 */
void new_proc(uid_t uid, gid_t egid, gid_t rgid, char *name);

/* get_proc()
 *  Searches the process table for a process with the given pid. Returns
 *  a pointer to the process struct if found, or NULL if not.
 */
struct proc *get_proc(pid_t pid);

/* sched()
 *  Determines which scheduling queue to add the given process to, and
 *  adds it.
 */
void sched(struct proc *p);

/* pick_proc()
 *  Picks a process to be run next. This starts with the head of queue 0, and
 *  proceeds to the only process in the lowest queue - IDLE. Once a runnable
 *  process is found, point next_proc to it and return.
 */
void pick_proc(void);

/* print_current_proc()
 *  Dumps information on the currently running process.
 */
void print_current_proc(void);

void check_current_proc(void);

#endif

