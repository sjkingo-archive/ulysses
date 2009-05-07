#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/sched.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <sys/types.h>

extern __volatile__ task_t *current_task; /* task.c */

pid_t new_kthread(void (*func)(void), char *name)
{
    TRACE_ONCE;

    /* Set up task */
    task_t *task = new_task(name);
    task->eip = (unsigned int)*func;
    task->ppid = current_task->pid;

    /* Set up thread */
    task->kthread = kmalloc(sizeof(kthread_t));
    task->kthread->state = STATE_STARTING;
    task->kthread->func = func;
    task->kthread->state = STATE_RUNNABLE;

    add_to_queue(task);
    return task->pid;
}

void kthread_yield(void)
{
    TRACE_ONCE;
    current_task->kthread->state = STATE_SLEEPING;
    change_current_task();
}

void kthread_exit(void)
{
    TRACE_ONCE;
    current_task->kthread->state = STATE_DESTROYING;
    task_exit();
}
