/* kernel/kthread.c - kernel threads library
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

#include <ulysses/gdt.h>
#include <ulysses/initrd.h>
#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/kprintf.h>
#include <ulysses/sched.h>
#include <ulysses/shell.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <string.h>

extern __volatile__ task_t *current_task; /* task.c */
static pid_t kthreadd_pid;

void kthreadd(void)
{
    TRACE_ONCE;
    kthreadd_pid = do_getpid();
    kprintf("kthreadd: running with pid %d\n", kthreadd_pid);
    kthread_create(run_initrd, "initrd");
    kthread_create(run_shell, "shell");
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
