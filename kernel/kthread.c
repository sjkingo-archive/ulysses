#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/sched.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <sys/types.h>

extern task_t *current_task; /* task.c */

pid_t new_kthread(void (*func)(void))
{
    TRACE_ONCE;
    task_t *task = new_task("kthread");
    task->eip = (unsigned int)*func;
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
