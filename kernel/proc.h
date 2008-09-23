
#ifndef _PROC_H
#define _PROC_H

#include <types.h>

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

#define NR_PROCS 100
struct proc proc[NR_PROCS]; /* the process table itself */

#define BEG_PROC (&proc[0]) /* pointer to the start of proc table */
#define END_PROC (&proc[NR_PROCS]) /* pointer to the end of the proc table */ 

/* Core kernel process pids */
#define PID_NONE -1
#define PID_IDLE 0

#define IDLE_P get_proc(PID_IDLE)

void init_proc(void);
struct proc *get_proc(pid_t pid);

#endif

