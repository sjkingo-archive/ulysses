
#include <ulysses/task.h>
#include <ulysses/sched.h>
#include <ulysses/kheap.h>
#include <ulysses/shutdown.h>
#include <ulysses/util.h>

#include <string.h>

task_t *current_task = NULL;
pid_t next_pid = 0;

extern page_dir_t *kernel_directory;
extern page_dir_t *current_directory;
extern unsigned int read_eip(void);

static pid_t new_pid(void)
{
    return next_pid++;
}

static void switch_task(void)
{
    unsigned int esp, ebp, eip;
    task_t *new;

    if (current_task == NULL) {
        return;
    }

    /* We need these registers later */
    __asm__ __volatile__("mov %%esp, %0" : "=r" (esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r" (ebp));

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
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;

    /* Ask the scheduler for a "new" task to run, or just return if we get the
     * same task back. This avoids a costly context switch for no reason.
     * Otherwise, update the current task.
     */
    new = pick_next_task();
    if (new == current_task) {
        return;
    } else {
        current_task = new;
    }

    /* Get the registers of the "new" task to run */
    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    /* Update the page directory */
    current_directory = current_task->page_dir;

    /* Perform the actual context switch:
     *  1. Disable interrupts: this is not reentrant,
     *  2. Temporarily put the new instruction pointer in a GP register,
     *  3. Load the stack and base pointers from the "new" task,
     *  4. Change the page directory,
     *  5. Put a dummy val in EAX so we recognise that we just task switched,
     *  6. Enable interrupts,
     *  7. Do nothing so the pipelined sti actually takes effect,
     *  8. Jumps to the instruction pointer we put in EAX, which "continues"
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

void init_task(void)
{
    /* Only allow this function to be called once */
    static flag_t initTask;
    if (initTask) panic("init_task() already set up");
    initTask = 1;

    current_task = new_task("init");
    add_to_queue(current_task);
}

task_t *new_task(char *name)
{
    task_t *t = (task_t *)kmalloc(sizeof(task_t));
    t->pid = new_pid();
    t->uid = 0;
    t->egid = 0;
    t->rgid = 0;
    t->name = (char *)kmalloc(strlen(name) + 1);
    strcpy(t->name, name);
    t->esp = 0;
    t->ebp = 0;
    t->eip = 0;
    t->page_dir = current_directory;
    t->ready = 1;
    t->s_ticks_left = SCHED_QUANTUM;
    t->s_quantum_size = SCHED_QUANTUM;
    t->next = NULL;
    return t;
}

pid_t fork(void)
{
    unsigned int eip;
    task_t *child, *parent;
    page_dir_t *page_dir;

    CLI;

    /* We need this later */
    parent = current_task;

    /* Clone the parent's page directory */
    page_dir = clone_dir(current_directory);

    /* Set up the child task */
    child = new_task(parent->name);
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
        
        __asm__ __volatile("mov %%esp, %0" : "=r" (esp));
        __asm__ __volatile("mov %%ebp, %0" : "=r" (ebp));

        child->esp = esp;
        child->ebp = ebp;
        child->eip = eip;
        
        STI;
        return child->pid;
    } else {
        /* Child, exit */
        STI;
        return 0;
    }
}

pid_t getpid(void)
{
    return current_task->pid;
}

void check_current_task(void)
{
    if (current_task == NULL) {
        return;
    }

    /* This is not reentrant */
    CLI;

    if (--current_task->s_ticks_left <= 0) {
#if TASK_DEBUG
        kprintf("check_current_task(): pid %d (%s) exceeded scheduling "
                "quanta\n", current_task->pid, current_task->name);
#endif

        /* Reset the quanta and switch tasks */
        current_task->s_ticks_left = current_task->s_quantum_size;
        switch_task();
    }
}

