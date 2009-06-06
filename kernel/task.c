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

#include "../config.h"
#include <ulysses/isr.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/gdt.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

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

    unlock_kernel();

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
    *--stack = (unsigned int)&ring3_exit; /* syscall: destroys task on return */
    /* bottom of stack */

    t->esp = (unsigned int)stack; /* the rest of the stack */
    t->ebp = (unsigned int)ebp;
}

static task_t *clone_task(task_t *parent)
{
    task_t *c = new_task(parent->name);
    c->ppid = parent->pid;
    c->uid = parent->uid;
    c->egid = parent->egid;
    c->rgid = parent->rgid;
    c->page_dir = clone_dir(parent->page_dir);
    kfree((unsigned int *)c->ebp);
    c->ebp = 0;
    c->esp = 0;
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

void kill_task(pid_t pid)
{
    TRACE_ONCE;
    task_t *t;

    /* Don't allow the kernel to be killed */
    if (pid == 0) {
        kprintf("kill: You can't kill the kernel.\n");
        return;
    }

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
    task_t *parent, *child;

    parent = current_task;
    child = clone_task(parent);

    child->eip = read_eip();
    if (current_task == parent) {
        /* Update the child's stack pointers */
        __asm__ __volatile("mov %%ebp, %0" : "=r" (child->ebp));
        __asm__ __volatile("mov %%esp, %0" : "=r" (child->esp));
        add_to_queue(child);
        return child->pid;
    } else {
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

#if !PREEMPT_KERNEL
    /* If we're not doing kernel preemption, just return to allow the
     * interrupted task to continue.
     * Note that there is no real way to *not* let the kernel task itself be
     * preempted, since it idles the CPU.
     */
    if (current_task->pid != 0 && current_task->ring == 0) {
        return;
    }
#endif

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
