#ifndef _KTHREAD_H
#define _KTHREAD_H

/* This is the kernel threads (kthreads) implementation.
 *
 * Kernel threads are non-preemptable. Instead we rely on the thread to 
 * yield the CPU. This greatly simplifies the threading code.
 *
 * Ideas and basic structure based on Jim Plank and Rich Wolski's kthreads
 * lecture, available at:
 * http://www.cs.utk.edu/~plank/plank/classes/cs560/560/notes/KThreads2/lecture.html
 */

#include <sys/types.h>

/* Kernel thread states -- a thread is always in one (and one only) of these
 * states. Apart from STATE_STARTING and STATE_DESTROYING, the transitions
 * between states are not restricted.
 *
 * STATE_STARTING
 *   The thread is currently being initialised.
 * STATE_RUNNABLE
 *   The thread is ready to run, and may be picked by the scheduler.
 * STATE_RUNNING
 *   The thread is currently running on the CPU and will stay in this state
 *   until it yields.
 * STATE_BLOCKED
 *   The thread is blocked waiting for a semaphore to return or a joined 
 *   thread to yield.
 * STATE_SLEEPING
 *   The thread has just yieled.
 * STATE_DESTROYING
 *   The thread is being destroyed.
 */
#define STATE_STARTING 1
#define STATE_RUNNABLE 2
#define STATE_RUNNING 3
#define STATE_BLOCKED 4
#define STATE_SLEEPING 5
#define STATE_DESTROYING 6

typedef struct kthread {
    void (*func)(void); /* function to call */
    unsigned int state; /* current state; see STATE_*, above */
} kthread_t;

pid_t new_kthread(void (*func)(void));

#endif
