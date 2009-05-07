#ifndef _ULYSSES_KTHREAD_H
#define _ULYSSES_KTHREAD_H

/* This is the kernel threads (kthreads) implementation. It defines basic
 * functions to deal with kthreads, but leaves all of the task management
 * to the tasker and scheduler.
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

/* new_kthread()
 *  Start a new kthread with the given function pointer as an entry point.
 *  This only sets up a new thread and calls the scheduler to manage it; it
 *  does not actually run the thread.
 */
pid_t new_kthread(void (*func)(void), char *name);

/* kthread_yield()
 *  Voluntarily yield the current kernel thread, allowing the task manager to 
 *  run another task.
 */
void kthread_yield(void);

/* kthread_exit()
 *  Quit the current kernel thread. This should only be called from inside a
 *  thread to exit itself.
 */
void kthread_exit(void);

#endif
