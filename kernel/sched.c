/* kernel/sched.c - task scheduler
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

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

#include <sys/types.h>

struct queue tasks_queue;

static void add_as_head(task_t *t)
{
    TRACE_ONCE;
    tasks_queue.head = t;
    tasks_queue.tail = NULL;
    t->next = NULL;

    if (kern.flags.debug_sched) {
        kprintf("add_as_head(): pid %d\n", t->pid);
    }
}

static void add_as_tail(task_t *t)
{
    TRACE_ONCE;
    if (tasks_queue.tail == NULL) {
        /* one task on queue */
        tasks_queue.tail = t;
        tasks_queue.head->next = t;
    } else {
        /* something already at tail exists, add as new tail */
        tasks_queue.tail->next = t;
        tasks_queue.tail = t;
    }

    t->next = NULL;

    if (kern.flags.debug_sched) {
        kprintf("add_as_tail(): pid %d\n", t->pid);
    }
}

static void move_to_tail(task_t *t)
{
    TRACE_ONCE;
    if (tasks_queue.head != t) {
        panic("Requested task be moved to tail when it was not the head!");
    }

    /* Since we are assuming the task is the head, just return if it has
     * no next pointer (hence the only task on the queue).
     */
    if (t->next == NULL) {
        return;
    }

    tasks_queue.head = t->next;
    if (kern.flags.debug_sched) {
        kprintf("move_to_tail(): pid %d removed from head\n", t->pid);
        kprintf("move_to_tail(): new head pid %d\n", tasks_queue.head->pid);
    }
    add_as_tail(t);
}

void init_sched(void)
{
    TRACE_ONCE;
    tasks_queue.head = NULL;
    tasks_queue.tail = NULL;
}

void add_to_queue(task_t *t)
{
    TRACE_ONCE;
    if (tasks_queue.head == NULL) {
        add_as_head(t);
    } else {
        add_as_tail(t);
    }
}

task_t *pick_next_task(void)
{
    TRACE_ONCE;
    task_t *t = tasks_queue.head;

    while (t != NULL) {
        if (t->ready) {
            if (kern.flags.debug_sched) {
                kprintf("pick_next_task(): pid %d ready\n", t->pid);
                dump_all_tasks();
            }
            move_to_tail(t);
            return t;
        } else {
            /* Remove from queue */
            if (kern.flags.debug_sched) {
                kprintf("pick_next_task(): pid %d not ready; removing from "
                        "queue and destroying\n", t->pid);
            }
            tasks_queue.head = t->next;
            free_task(t);
            t = tasks_queue.head;
        }
    }

    return NULL;
}

void dump_all_tasks(void)
{
    TRACE_ONCE;
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d ", t->pid);
        t = t->next;
    }
    kprintf("\n");
}

task_t *get_task(pid_t pid)
{
    TRACE_ONCE;
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        if (t->pid == pid) {
            return t;
        }
        t = t->next;
    }
    return NULL;
}
