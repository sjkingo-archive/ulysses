/* kernel/task.c - task management
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/isr.h>
#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/gdt.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>
#include <ulysses/util_x86.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>

__volatile__ task_t *current_task = NULL;
pid_t next_pid = 0;

extern page_dir_t *kernel_directory;
extern page_dir_t *current_directory;
extern unsigned int read_eip(void);

/* syscall_trap.s: This is used as an exit path from ring 3. */
extern void ring3_exit(void);

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

    lock_kernel();

    /* We need these registers later */
    if (save) {
        esp = READ_ESP();
        ebp = READ_EBP();
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
    
    if (kern.flags.debug_task) {
        kprintf("switch_task(): switching to pid %d (%s); eip %p; ebp %p; "
                "esp %p\n", current_task->pid, current_task->name,
                (unsigned int *)eip, (unsigned int *)ebp, 
                (unsigned int *)esp);
    }

    /* We need to do a couple of other things if this is a kernel thread */
    if (current_task->kthread != NULL) {
        kthread_running();
    }
   
    /* Perform the actual context switch:
     *  1. Temporarily put the new instruction pointer in a GP register,
     *  2. Load the stack and base pointers from the "new" task,
     *  3. Change the page directory,
     *  4. Put a dummy val in EAX so we recognise that we just task switched,
     *  5. Enable interrupts,
     *  6. Do nothing so the pipelined sti actually takes effect,
     *  7. Jumps to the instruction pointer we put in ECX, which "continues"
     *     executing the "new" task.
     */
    __asm__ __volatile__("       \
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

/* setup_stack()
 *  Create the default stack for the given task. The stack is 4 KB that grows
 *  backward -- that is, grows to a *lower* memory address. This is why we
 *  set esp to the end of the allocated area and grow it backwards.
 */
static void setup_stack(task_t *t)
{
    TRACE_ONCE;
    unsigned int *esp = kmalloc(0x1000) + 0x1000; /* 4 KB stack, at top */
    unsigned int *stack = esp;

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
    *--stack = (unsigned int)&ring3_exit; /* syscall: destroys task on return */
    /* bottom of stack */

    t->esp = (unsigned int)stack; /* the rest of the stack */
    t->ebp = (unsigned int)esp;
}

static task_t *clone_task(task_t *parent)
{
    task_t *c = new_task(parent->name);
    c->ppid = parent->pid;
    c->uid = parent->uid;
    c->egid = parent->egid;
    c->rgid = parent->rgid;

    /* We aren't using the default stack */
    kfree((void *)c->ebp);
    c->ebp = 0;
    c->esp = 0;

    /* Clone the parent's page directory */
    c->page_dir = clone_dir(parent->page_dir);
    return c;
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
    t->eip = (unsigned int)&ring3_exit; /* default to just exiting */
    t->page_dir = current_directory;
    t->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
    t->ready = 1;
    t->s_ticks_left = SCHED_QUANTUM;
    t->s_quantum_size = SCHED_QUANTUM;
    t->cpu_time = 0;
    t->kthread = NULL;
    t->ring = 0;
    t->next = NULL;
    setup_stack(t);
    return t;
}

void free_task(task_t *task)
{
    TRACE_ONCE;
    kfree(task->name);
    kfree(task->kernel_stack);
    if (task->kthread != NULL) {
        kfree(task->kthread);
        task->kthread = NULL;
    }
    kfree(task);
}

void task_exit(void)
{
    TRACE_ONCE;
    current_task->ready = 0; /* tell the scheduler to remove this task */
    switch_task(FALSE); /* don't save state */
}

void kill_task(pid_t pid)
{
    TRACE_ONCE;
    task_t *t;

    /* Try and find the task */
    t = get_task(pid);
    if (t == NULL) {
        kprintf("kill: No such task (pid %d)\n", pid);
        return;
    }

    /* Only allow users to kill their own tasks; except for root */
    if (t->uid != 0 && (t->uid != do_getuid())) {
        kprintf("kill: Permission denied.\n");
        return;
    }
    
    t->ready = 0;
}

void kill_all_tasks(void)
{
    TRACE_ONCE;
    unsigned int i;
    for (i = next_pid - 1; i > 0; i--) {
        task_t *t = get_task(i);
        if (t != NULL) {
            t->ready = 0;
        }
    }
}

pid_t do_fork(void)
{
    TRACE_ONCE;
    unsigned int eip;
    task_t *child, *parent;

    /* Kernel threads cannot fork as they share an address space with their 
     * parent and we have no means of cloning this.
     */
    if (current_task->kthread != NULL) {
        errno = ECANCELED;
        return -1;
    }

    lock_kernel();

    /* Clone the parent */
    parent = current_task;
    child = clone_task(parent);
    if (kern.flags.debug_task) {
        kprintf("do_fork() parent: new child \"%s\", pid %d, uid %d\n", 
                child->name, child->pid, child->uid);
    }

    /* Entry point for the new task */
    eip = read_eip(); /* both tasks will continue executing from here */
    if (current_task == parent) {
        /* Parent, set up the pointers for the child */
        child->esp = READ_ESP();
        child->ebp = READ_EBP();
        child->eip = eip;
        
        /* Tell the scheduler about this task */
        add_to_queue(child);

        /* We only need to unlock the kernel here since we're returning now */
        unlock_kernel();
        return child->pid;
    } else {
        /* Child */
        return 0;
    }
}

pid_t do_getpid(void)
{
    TRACE_ONCE;
    return current_task->pid;
}

uid_t do_getuid(void)
{
    TRACE_ONCE;
    return current_task->uid;
}

void check_current_task(void)
{
    TRACE_ONCE;
    if (current_task == NULL) {
        return;
    }

    /* If we're not doing kernel preemption, just return to allow the
     * interrupted task to continue.
     * Note that there is no real way to *not* let the kernel task itself be
     * preempted, since it idles the CPU.
     */
    if (!kern.flags.preempt_kernel && current_task->pid != 0 && 
            current_task->ring == 0) {
        return;
    }

    if (--current_task->s_ticks_left <= 0) {
        if (kern.flags.debug_task) {
            kprintf("check_current_task(): pid %d (%s) exceeded scheduling "
                    "quanta\n", current_task->pid, current_task->name);
        }

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

void set_current_ring3(void)
{
    TRACE_ONCE;
    current_task->ring = 3;
}

void update_cpu_time(void)
{
    TRACE_ONCE;
    current_task->cpu_time++;
}

void change_name(const char *new_name)
{
    TRACE_ONCE;
    kfree(current_task->name);
    current_task->name = (char *)kmalloc(strlen(new_name) + 1);
    strcpy(current_task->name, new_name);
}
