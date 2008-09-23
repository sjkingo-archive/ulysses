
#ifndef _PROC_H
#define _PROC_H

#include <types.h>

/* An entry in the process table */
struct proc {
    pid_t pid;
    uid_t uid;
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    unsigned short sched_q; /* scheduling queue this proc belongs to */

    unsigned short s_ticks_left; /* number of scheduling ticks left */
    unsigned short s_quantum_size; /* quantum size in ticks */
};
#define NR_PROCS 100
struct proc proc[NR_PROCS]; /* the process table itself */

#define BEG_PROC (&proc[0]) /* pointer to the start of proc table */
#define END_PROC (&proc[NR_PROCS]) /* pointer to the end of the proc table */ 

/* Core kernel proccess pids */
#define PID_NONE -1
#define PID_IDLE 0

#define IDLE_P get_proc(PID_IDLE)

/* init_proc()
 *  Initialises the process table. This needs to be called *once* by kmain(),
 *  and never again.
 */
void init_proc(void);

/* get_proc()
 *  Searches the process table for a process with the given pid. Returns
 *  a pointer to the process struct if found, or NULL if not.
 */
struct proc *get_proc(pid_t pid);

#endif

