#include <ulysses/gdt.h>
#include <ulysses/initrd.h>
#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/sched.h>
#include <ulysses/shell.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <string.h>
#include <unistd.h>

extern __volatile__ task_t *current_task; /* task.c */
static pid_t kthreadd_pid;

static void test_ring3(void)
{
    TRACE_ONCE;
    char *msg = "Hello via SYS_WRITE in usermode; now going into a busy "
            "loop to test preemption.";
    unsigned int len = strlen(msg);

    switch_to_ring3();
    syscall0(SYS_DUMMY);
    write(STDOUT_FILENO, msg, len);
    while (1); /* spin a bit */
}

void kthreadd(void)
{
    TRACE_ONCE;
    kthreadd_pid = do_getpid();
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

pid_t kthread_clone(void)
{
    TRACE_ONCE;
    unsigned int eip;
    task_t *task, *parent;

    parent = current_task;

    /* Set up the new task */
    task = new_task(parent->name);
    task->uid = parent->uid;
    task->egid = parent->egid;
    task->rgid = parent->rgid;
    task->kthread = kmalloc(sizeof(kthread_t));
    task->kthread->state = STATE_RUNNABLE;
    add_to_queue(task);

    /* Entry point for the new task */
    eip = read_eip();
    if (current_task == parent) {
        /* Parent, set up the pointers for the child */
        unsigned int esp, ebp;

        __asm__ __volatile("mov %%ebp, %0" : "=r" (ebp));
        __asm__ __volatile("mov %%esp, %0" : "=r" (esp));

        task->esp = esp;
        task->ebp = ebp;
        task->eip = eip;

        return task->pid;
    } else {
        /* Child */
        return 0;
    }
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
