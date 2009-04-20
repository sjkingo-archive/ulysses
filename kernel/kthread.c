#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/sched.h>
#include <ulysses/task.h>

#include <sys/types.h>

pid_t new_kthread(void (*func)(void))
{
    task_t *task = new_task("kthread");
    task->eip = (unsigned int)&func;
    task->kthread = kmalloc(sizeof(kthread_t));
    task->kthread->state = STATE_STARTING;
    task->kthread->func = func;
    task->kthread->state = STATE_RUNNABLE;
    add_to_queue(task);
}

