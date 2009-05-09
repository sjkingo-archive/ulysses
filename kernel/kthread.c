#include <ulysses/gdt.h>
#include <ulysses/initrd.h>
#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/sched.h>
#include <ulysses/shell.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <unistd.h>

extern __volatile__ task_t *current_task; /* task.c */
static pid_t kthreadd_pid;

static void test_ring3(void)
{
    TRACE_ONCE;
    switch_to_ring3();
    syscall0(SYS_DUMMY);
    write(STDOUT_FILENO, "Hello via SYS_WRITE in usermode", 31);
}

void kthreadd(void)
{
    TRACE_ONCE;
    kthreadd_pid = getpid();
    kthread_create(run_initrd, "initrd");
    kthread_create(run_shell, "shell");
    kthread_create(test_ring3, "test_ring3");
    while (1) kthread_yield(); /* waiting for work */
}

pid_t kthread_create(void (*func)(void), const char *name)
{
    TRACE_ONCE;

    /* Set up task */
    task_t *task = new_task(name);
    task->eip = (unsigned int)*func;
    task->ppid = kthreadd_pid;

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
    exit();
}

void kthread_running(void)
{
    TRACE_ONCE;
    current_task->kthread->state = STATE_RUNNING;
}
