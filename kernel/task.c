#include "../config.h"
#include <ulysses/isr.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/gdt.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <string.h>
#include <sys/types.h>

__volatile__ task_t *current_task = NULL;
pid_t next_pid = 0;

extern page_dir_t *kernel_directory;
extern page_dir_t *current_directory;
extern unsigned int read_eip(void);

static pid_t new_pid(void)
{
    TRACE_ONCE;
    return next_pid++;
}

static void switch_task(flag_t save)
{
    TRACE_ONCE;
    unsigned int esp, ebp, eip;

    if (current_task == NULL) {
        return;
    }

    /* We need these registers later */
    if (save) {
        __asm__ __volatile__("mov %%esp, %0" : "=r" (esp));
        __asm__ __volatile__("mov %%ebp, %0" : "=r" (ebp));
    }

    /* Read the current instruction pointer. Two things could have happened 
     * when read_eip is called -- either the eip is returned as requested, or
     * a task switch has just occured since we jumped back to eax.
     * The latter requires us to return immediately, so we check for the dummy
     * value set on the switch.
     */
    eip = read_eip();
    if (eip == 0x12345) {
        return;
    }

    /* Save the the registers of the current task */
    if (save) {
        current_task->eip = eip;
        current_task->esp = esp;
        current_task->ebp = ebp;
    }

    current_task = pick_next_task();

    /* Get the registers of the "new" task to run */
    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    /* Update the page directory and kernel stack */
    current_directory = current_task->page_dir;
    switch_kernel_stack();
    
#if TASK_DEBUG
    kprintf("switch_task(): switching to pid %d (%s)\n", current_task->pid,
            current_task->name);
#endif

    /* We need to do a couple of other things if this is a kernel thread */
    if (current_task->kthread != NULL) {
        kthread_running();
    }

    /* Perform the actual context switch:
     *  1. Firstly disable interrupts since this isn't reentrant,
     *  2. Temporarily put the new instruction pointer in a GP register,
     *  3. Load the stack and base pointers from the "new" task,
     *  4. Change the page directory,
     *  5. Put a dummy val in EAX so we recognise that we just task switched,
     *  6. Enable interrupts,
     *  7. Do nothing so the pipelined sti actually takes effect,
     *  8. Jumps to the instruction pointer we put in ECX, which "continues"
     *     executing the "new" task.
     */
    __asm__ __volatile__("       \
            cli;                 \
            mov %0, %%ecx;       \
            mov %1, %%esp;       \
            mov %2, %%ebp;       \
            mov %3, %%cr3;       \
            mov $0x12345, %%eax; \
            sti;                 \
            nop;                 \
            jmp *%%ecx"
            : : "r" (eip), "r" (esp), "r" (ebp), 
            "r" (current_directory->phys_addr));
    
    /* the jmp should mean we never get here */
    panic("Context switch failed");
}

static void setup_stack(task_t *t)
{
    TRACE_ONCE;
    /* We offset the stack for flags register */
    unsigned int *ebp = kmalloc(2048); /* bottom of stack */
    unsigned int *stack = ebp + 2048; /* we need 2KB for defaults */

    *--stack = 0x202;       /* EFLAGS */
    *--stack = 0x08;        /* CS */
    *--stack = t->eip;      /* EIP */

    *--stack = 0;           /* EDI */
    *--stack = 0;           /* ESI */
    *--stack = 0;           /* EBP */
    *--stack = 0;           /* NULL */

    *--stack = 0;           /* EBX */
    *--stack = 0;           /* EDX */
    *--stack = 0;           /* ECX */
    *--stack = 0;           /* EAX */

    *--stack = 0x10;        /* DS */
    *--stack = 0x10;        /* ES */
    *--stack = 0x10;        /* FS */
    *--stack = 0x10;        /* GS */
    *--stack = (unsigned int)&do_exit; /* syscall: destroys task on return */
    /* bottom of stack */

    t->esp = (unsigned int)stack; /* the rest of the stack */
    t->ebp = (unsigned int)ebp;
}

void init_task(void)
{
    TRACE_ONCE;
    /* Only allow this function to be called once */
    static flag_t initTask;
    if (initTask) panic("init_task() already set up");
    initTask = 1;

    /* The kernel task is the execution thread for kernel startup. It will 
     * continue to start the kernel and will always get picked to idle the
     * CPU if there is nothing else to do.
     */
    current_task = new_task("kernel");
    add_to_queue((task_t *)current_task);
}

task_t *new_task(const char *name)
{
    TRACE_ONCE;
    task_t *t = (task_t *)kmalloc(sizeof(task_t));
    t->pid = new_pid();
    t->ppid = 0;
    t->uid = 0;
    t->egid = 0;
    t->rgid = 0;
    t->name = (char *)kmalloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->esp = 0;
    t->ebp = 0;
    t->eip = 0;
    t->page_dir = current_directory;
    t->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
    t->ready = 1;
    t->s_ticks_left = SCHED_QUANTUM;
    t->s_quantum_size = SCHED_QUANTUM;
    t->kthread = NULL;
    t->next = NULL;
    setup_stack(t);
    return t;
}

void free_task(task_t *task)
{
    TRACE_ONCE;
    return; /* XXX this function seems buggy and causes a page fault the next 
               time kmalloc_a() is called.
            */
    kfree(task->name);
    kfree(task->kernel_stack);
    if (task->kthread != NULL) {
        kfree(task->kthread);
        task->kthread = NULL;
    }
    /* XXX we can't free the stack or a page fault will occur */
    kfree(task);
}

void task_exit(void)
{
    TRACE_ONCE;
    current_task->ready = 0; /* tell the scheduler to remove this task */
    switch_task(FALSE); /* don't save state */
}

pid_t fork(void)
{
    TRACE_ONCE;
    unsigned int eip;
    task_t *child, *parent;
    page_dir_t *page_dir;

    /* This is really important code that can't be interrupted! */
    CLI;

    /* We need this later */
    parent = current_task;

    /* Clone the parent's page directory */
    page_dir = clone_dir(current_directory);

    /* Set up the child task */
    child = new_task(parent->name);
    child->ppid = parent->pid;
    child->uid = parent->uid;
    child->egid = parent->egid;
    child->rgid = parent->rgid;
    child->page_dir = page_dir;

#if TASK_DEBUG
    kprintf("fork() parent: new child \"%s\", pid %d, uid %d, egid %d, "
            "rgid %d\n", child->name, child->pid, child->uid, child->egid,
            child->rgid);
#endif

    /* Tell the scheduler about this task */
    add_to_queue(child);

    /* Entry point for the new task */
    eip = read_eip();
    if (current_task == parent) {
        /* Parent, set up the pointers for the child */
        unsigned int esp, ebp;
        
        __asm__ __volatile("mov %%ebp, %0" : "=r" (ebp));
        __asm__ __volatile("mov %%esp, %0" : "=r" (esp));

        child->esp = esp;
        child->ebp = ebp;
        child->eip = eip;
        
        STI;
        return child->pid;
    } else {
        /* Child, exit */
        return 0;
    }
}

pid_t getpid(void)
{
    TRACE_ONCE;
    return current_task->pid;
}

void check_current_task(void)
{
    TRACE_ONCE;
    if (current_task == NULL) {
        return;
    }

    if (--current_task->s_ticks_left <= 0) {
#if TASK_DEBUG
        kprintf("check_current_task(): pid %d (%s) exceeded scheduling "
                "quanta\n", current_task->pid, current_task->name);
#endif

        /* Reset the quanta and switch tasks */
        current_task->s_ticks_left = current_task->s_quantum_size;
        change_current_task();
    }
}

void switch_kernel_stack(void)
{
    TRACE_ONCE;
    set_kernel_stack(current_task->kernel_stack + KERNEL_STACK_SIZE);
}

void change_current_task(void)
{
    TRACE_ONCE;
    switch_task(TRUE);
}

