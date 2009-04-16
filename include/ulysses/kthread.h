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

#define MAX_KTHREADS 100

typedef struct kernel_thread {
    void (*func)(void); /* function to call */
    unsigned int tid; /* thread id */
    unsigned int state; /* current state; see STATE_*, above */
} kt;

/* init_kthread()
 *  Initialise kernel threads ready for use.
 */
void init_kthread(void);

/* kthread_fork()
 *  Create a new kernel thread and schedule it for running. The thread's
 *  state will be set to STATE_STARTING. Returns the thread's ID.
 */
unsigned int kthread_fork(void (*func)(void));

/* kthread_destory()
 *  Destroys the given kernel thread (or current thread if NULL is given).
 *  If the currently running thread calls this, yield first.
 */
void kthread_destroy(kt *kthread);

/* kthread_yield()
 *  Yields the currently running thread and allows a new thread to be run.
 *  This only makes sense to be called by the currently running thread.
 */
void kthread_yield(void);

/* kthread_block()
 *  Blocks the given thread (or current thread if NULL is given), and allows
 *  a new thread to be run.
 */
void kthread_block(kt *kthread);

/* kthread_whoami()
 *  Returns a pointer to the currently executing thread struct.
 */
kt *kthread_whoami(void);

/* kthread_spin()
 *  Spin 10 times and yield.
 */
void kthread_spin(void);

/* kthread_run()
 *  Pick a kernel thread in the RUNNABLE state and run it. If there are no
 *  threads in the runnable state, do nothing and silently return.
 */
void kthread_run(void);

/* dump_kthreads()
 *  Dump information on all known kernel threads.
 */
void dump_kthreads(void);

#endif
