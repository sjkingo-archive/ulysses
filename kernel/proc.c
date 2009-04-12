
#include "kernel.h"
#include "proc.h"

#include <string.h>

proc_t *current_proc = NULL;
pid_t next_pid = 0;

extern page_dir_t *kernel_directory;
extern page_dir_t *current_directory;
extern unsigned int read_eip(void);

static pid_t new_pid(void)
{
    return next_pid++;
}

static void sched(proc_t *p)
{
    /* Pick the queue and update the process */
    if (p->uid == 0) {
        p->sched_q = ROOT_Q;
    } else {
        p->sched_q = USER_Q;
    }

    /* Ensure we're not going out of bounds by a misconfig in proc.h */
    if (p->sched_q >= NR_SCHED_Q) {
        panic("Assigned scheduling queue is out of range");
    }

    p->next = NULL;
    if (sched_queues[p->sched_q].head == NULL) {
        /* assume empty queue */
        sched_queues[p->sched_q].head = p;
        sched_queues[p->sched_q].tail = NULL;
#if SCHED_DEBUG
        kprintf("sched(): pid %d added as new head of queue %d\n", p->pid, 
                p->sched_q);
#endif
    } else if (sched_queues[p->sched_q].tail == NULL) {
        /* one proc */
        sched_queues[p->sched_q].tail = p;
        sched_queues[p->sched_q].head->next = p;
#if SCHED_DEBUG
        kprintf("sched(): pid %d added as new tail of queue %d\n", p->pid, 
                p->sched_q);
#endif
    } else {
        /* non-empty queue */
        sched_queues[p->sched_q].tail->next = p;
        sched_queues[p->sched_q].tail = p;
#if SCHED_DEBUG
        kprintf("sched(): pid %d added to tail of queue %d\n", p->pid, 
                p->sched_q);
#endif
    }

}

#if 0
static void switch_proc(proc_t *p)
{
    if (current_proc == NULL || p == NULL) {
        return;
    }

    unsigned int esp, ebp, eip;
    __asm__ __volatile__("mov %%esp, %0" : "=r" (esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r" (ebp));
    eip = read_eip();

    /* If we have just switched tasks, return immediately to avoid
     * switching again.
     */
    if (eip == 0x12345) {
        return;
    }

    /* Save the current process' pointers for resuming later */
    current_proc->eip = eip;
    current_proc->esp = esp;
    current_proc->ebp = ebp;

    /* This is now the new process */
    current_proc = p;

    /* These are the pointers that we need to resume */
    eip = current_proc->eip;
    esp = current_proc->esp;
    ebp = current_proc->ebp;

    /* Switch page dirs */
    current_directory = current_proc->page_dir;

    __asm__ __volatile__("   \
        cli;                 \
        mov %0, %%ecx;       \
        mov %1, %%esp;       \
        mov %2, %%ebp;       \
        mov %3, %%cr3;       \
        mov $0x12345, %%eax; \
        sti;                 \
        jmp *%%ecx           "
        : : "r" (eip), "r" (esp), "r" (ebp), "r" 
        (current_directory->phys_addr));
}
#endif

void switch_proc(void)
{
    // If we haven't initialised tasking yet, just return.
    //if (!p)
    //    return;

    // Read esp, ebp now for saving later on.
    unsigned int esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    // Read the instruction pointer. We do some cunning logic here:
    // One of two things could have happened when this function exits - 
    //   (a) We called the function and it returned the EIP as requested.
    //   (b) We have just switched tasks, and because the saved EIP is essentially
    //       the instruction after read_eip(), it will seem as if read_eip has just
    //       returned.
    // In the second case we need to return immediately. To detect it we put a dummy
    // value in EAX further down at the end of this function. As C returns values in EAX,
    // it will look like the return value is this dummy value! (0x12345).
    eip = read_eip();

    // Have we just switched tasks?
    if (eip == 0x12345)
        return;

    // No, we didn't switch tasks. Let's save some register values and switch.
    current_proc->eip = eip;
    current_proc->esp = esp;
    current_proc->ebp = ebp;

    // Get the next task to run.
    //current_task = current_task->next;
    // If we fell off the end of the linked list start again at the beginning.
    //if (!current_task) current_task = ready_queue;

    eip = current_proc->eip;
    esp = current_proc->esp;
    ebp = current_proc->ebp;

    // Make sure the memory manager knows we've changed page directory.
    current_directory = current_proc->page_dir;
    // Here we:
    // * Stop interrupts so we don't get interrupted.
    // * Temporarily puts the new EIP location in ECX.
    // * Loads the stack and base pointers from the new task struct.
    // * Changes page directory to the physical address (physicalAddr) of the new directory.
    // * Puts a dummy value (0x12345) in EAX so that above we can recognise that we've just
    //   switched task.
    // * Restarts interrupts. The STI instruction has a delay - it doesn't take effect until after
    //   the next instruction.
    // * Jumps to the location in ECX (remember we put the new EIP in there).
    asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx           "
                 : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->phys_addr));
}


static void pick_proc(void)
{
    unsigned short i;
    proc_t *p;

    /* Run a kernel thread if one is ready */
    kthread_run();

    for (i = 0; i < NR_SCHED_Q; i++) {
        p = sched_queues[i].head; /* head of the queue */
        if (p == NULL || !p->ready) {
            continue;
        }

#if SCHED_DEBUG
        kprintf("pick_proc(): switching to pid %d (\"%s\") from queue %d\n", 
                p->pid, p->name, i);
#endif
        switch_proc();
        return; /* shouldn't be here anyway */
    }
}

void init_proc(void)
{
    /* Only allow this function to be called once */
    static flag_t initProc;
    if (initProc) panic("init_proc() already set up");
    initProc = 1;
    
    __asm__ __volatile__("cli");
#if 0
    /* Move the stack to a known location */
    move_stack((void *)0xE0000000, 0x2000);
#if DEBUG
    kprintf("init_proc(): kernel stack moved to 0xE0000000\n");
#endif
#endif
    /* Blank the scheduling queues */
    unsigned short i;
    for (i = 0; i < NR_SCHED_Q; i++) {
        sched_queues[i].q_num = i;
        sched_queues[i].head = NULL;
        sched_queues[i].tail = NULL;
    }

    /* Create the kernel process */
    current_proc = (proc_t *)kmalloc(sizeof(proc_t));
    current_proc->pid = new_pid();
    current_proc->uid = 0;
    current_proc->egid = 0;
    current_proc->rgid = 0;
    current_proc->name = (char *)kmalloc(strlen("kernel") + 1);
    strcpy(current_proc->name, "kernel");
    current_proc->esp = 0;
    current_proc->ebp = 0;
    current_proc->eip = 0;
    current_proc->page_dir = current_directory;
    current_proc->ready = 1;
    current_proc->s_ticks_left = SCHED_QUANTUM;
    current_proc->s_quantum_size = SCHED_QUANTUM;
    current_proc->next = NULL;

#if DEBUG
    kprintf("init_proc(): proc table set up\n");
#endif

    __asm__ __volatile__("sti");
}

pid_t fork(void)
{
    __asm__ __volatile__("cli");

    unsigned int eip;
    proc_t *child, *parent;
    page_dir_t *page_dir;

    parent = current_proc;

    page_dir = clone_dir(current_directory);

    /* Set up the child process */
    child = (proc_t *)kmalloc(sizeof(proc_t));
    child->pid = new_pid();
    child->uid = parent->uid;
    child->egid = parent->egid;
    child->rgid = parent->rgid;
    child->name = (char *)kmalloc(strlen(parent->name) + 1);
    strcpy(child->name, parent->name);
    child->esp = 0;
    child->ebp = 0;
    child->eip = 0;
    child->page_dir = page_dir;
    child->ready = 1;
    child->s_ticks_left = SCHED_QUANTUM;
    child->s_quantum_size = SCHED_QUANTUM;
    child->next = NULL;

#if SCHED_DEBUG
    kprintf("fork() parent: new child \"%s\", pid %d, uid %d, egid %d, "
            "rgid %d\n", child->name, child->pid, child->uid, child->egid,
            child->rgid);
#endif

    /* Add the process to the kernel scheduler */
    sched(child);

    /* Entry point for the new process */
    eip = read_eip();

    if (current_proc == parent) {
        /* Parent, set up the pointers for the child */
        unsigned int esp, ebp;
        
        __asm__ __volatile("mov %%esp, %0" : "=r" (esp));
        __asm__ __volatile("mov %%ebp, %0" : "=r" (ebp));

        child->esp = esp;
        child->ebp = ebp;
        child->eip = eip;
        
        __asm__ __volatile__("sti");
        return child->pid;
    } else {
        /* Child, exit */
        return 0;
    }
}

pid_t getpid(void)
{
    return current_proc->pid;
}


void check_current_proc(void)
{
    if (current_proc == NULL) {
        return;
    }

    if (--current_proc->s_ticks_left <= 0) {
        if (current_proc->pid == 0) {
            /* kernel task */
            kprintf("current task is 0, picking a new\n");
            pick_proc();
            return;
        }
#if 0
            kprintf("current %d != head %d\n", current_proc->pid,
                    sched_queues[current_proc->sched_q].head->pid);
            panic("current_proc not at head of scheduling queue");
        }
#endif

        dump_queues();

#if SCHED_DEBUG
        kprintf("check_current_proc(): pid %d (%s) exceeded scheduling "
                "quanta\n", current_proc->pid, current_proc->name);
#endif
        current_proc->s_ticks_left = current_proc->s_quantum_size;

        /* Move process from head to tail */
        if (sched_queues[current_proc->sched_q].tail == NULL) {
            kprintf("not moving only proc in list to tail\n");
        } else {
            sched_queues[current_proc->sched_q].head = current_proc->next;
            sched_queues[current_proc->sched_q].tail->next = current_proc;
            sched_queues[current_proc->sched_q].tail = current_proc;
            current_proc->next = NULL;
#if SCHED_DEBUG
            kprintf("check_current_proc(): moved pid %d to tail of queue\n", 
                    current_proc->pid);
#endif
        }

        /* Reschedule the process and pick another */
        //sched(current_proc);
        pick_proc();
    }
}

void dump_queues(void)
{
    int i;
    for (i = 0; i < NR_SCHED_Q; i++) {
        kprintf("sched_queue %d:\n\t", i);
        proc_t *p = sched_queues[i].head;
        int j = 0;
        while (p != NULL) {
            if (j++ > 10) break;
            kprintf("%d ", p->pid);
            p = p->next;
        }
        kprintf("\n");
    }
}
