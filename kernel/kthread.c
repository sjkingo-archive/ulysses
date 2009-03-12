
#include "kernel.h"
#include "kthread.h"

#include <sys/types.h>

kt *current; /* pointer to current thread on CPU */
kt *all_kthreads[MAX_KTHREADS]; /* array of pointers to all threads */
unsigned int next_index; /* next index to use in array */
unsigned int next_tid; /* next thread id to use */

/* add_to_list()
 *  Add the given kernel thread to the array of known threads.
 */
static void add_to_list(kt *thread)
{
    if (next_index >= MAX_KTHREADS) {
        panic("Reached maximum limit of kthreads");
    }
    all_kthreads[next_index++] = thread;
}

/* remove_from_list()
 *  Remove the given thread from the array of known threads.
 */
static flag_t remove_from_list(kt *kthread)
{
    /* This should only be done on kthreads that are being destroyed */
    if (kthread->state != STATE_DESTROYING) return FALSE;

    unsigned int i;
    for (i = 0; i < MAX_KTHREADS; i++) {
        if (all_kthreads[i]->tid == kthread->tid) {
            all_kthreads[i] = NULL; /* this will be freed after return */
            next_index = i; /* next thread should replace this in array */
            return TRUE;
        }
    }

#if DEBUG
    kprintf("kthread_destroy(): did not find thread in list?\n");
#endif
    return FALSE;
}

/* new_thread()
 *  Set up a new kernel thread struct and return a pointer to it.
 */
static kt *new_thread(void (*func)(void))
{
    kt *thread;

    /* Allocate space for a new thread struct */
    thread = (kt *)kmalloc(sizeof(struct kernel_thread));
    if (thread == NULL) return NULL;

    thread->state = STATE_STARTING;
    thread->func = func;
    thread->tid = next_tid++;
    add_to_list(thread); /* keep track of the kthread */

    return thread;
}

void init_kthread(void)
{
    /* Only allow this function to be called once */
    static flag_t initKthread;
    if (initKthread) panic("init_kthread() already set up");
    initKthread = TRUE;

    /* Initialise all the globals */
    unsigned int i;
    for (i = 0; i < MAX_KTHREADS; i++) {
        all_kthreads[i] = NULL;
    }

    current = NULL;
    next_tid = 0;
    next_index = 0;
}

unsigned int kthread_fork(void (*func)(void))
{
    kt *thread = new_thread(func);
    if (thread == NULL) {
        panic("kthread_fork() failed - not enough space in memory");
    }

    thread->state = STATE_RUNNABLE;
    return thread->tid;
}

void kthread_destroy(kt *kthread)
{
    unsigned int tid = kthread->tid;

    /* If we are the currently running thread, yield */
    if (kthread == current && kthread->state == STATE_RUNNING) {
        kthread_yield();
    }

    kthread->state = STATE_DESTROYING;

    /* Attempt to remove from array of known threads */
    if (!remove_from_list(kthread)) {
        kprintf("kthread_destroy(): tid %d is not in STATE_DESTROYING\n", 
                tid);
        return;
    }

    kfree(kthread);
    kthread = NULL; 
#if DEBUG
    kprintf("kthread_destroy(): tid %d destroyed\n", tid);
#endif
}

void kthread_yield(void)
{
    current->state = STATE_RUNNABLE;
}

void kthread_block(kt *kthread)
{
    if (kthread == NULL) kthread = kthread_whoami();
    kthread->state = STATE_BLOCKED;
}

kt *kthread_whoami(void)
{
    return current;
}

void kthread_spin(void)
{
    unsigned int i = 0;
    while ((i++) < 10);
    kprintf("%d yielded\n", current->tid);
    kthread_yield();
}

void kthread_run(void)
{
    /* This is re-entrant, so keep a state of where in the list we last were 
     * when a runnable process was found. This allows us to continue when 
     * the scheduler calls us again, rather than starting with thread 0 every
     * time (and starving the other threads).
     */
    static unsigned int i = 0;

    while (i < MAX_KTHREADS) {
        if (all_kthreads[i] != NULL && 
                all_kthreads[i]->state == STATE_RUNNABLE) {
            current = all_kthreads[i]; /* this is now the current thread */
            all_kthreads[i]->state = STATE_RUNNING;
            all_kthreads[i]->func(); /* run it */

            /* The thread yielded, pick the next thread when the scheduler
             * calls us again.
             */
            i++;
            return;
        } else {
            i++; /* try next thread */
        }
    }

    i = 0; /* reset to thread 0 */
}

void dump_kthreads(void)
{
    unsigned int i = 0;
    for (i = 0; i < MAX_KTHREADS; i++) {
        if (all_kthreads[i] == NULL) continue;
        kprintf("thread tid %d, index %d, state %d\n", all_kthreads[i]->tid,
                i, all_kthreads[i]->state);
    }
}

