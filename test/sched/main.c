/* test/sched/main.c - scheduler tester
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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/* Cruff needed for the task_t struct */
typedef void page_dir_t;
typedef int flag_t;
struct kthread { };

struct kernel_flags {
    flag_t preempt_kernel;
    flag_t debug_sched;
    flag_t debug_task;
    flag_t debug_interrupt;
    flag_t debug_ticks;
};
struct kernel {
    struct kernel_flags flags; /* parsed flags from kernel command line */
};
struct kernel kern;

typedef struct task {
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t egid;
    gid_t rgid;
    char *name;
    unsigned int esp;
    unsigned int ebp;
    unsigned int eip;
    page_dir_t *page_dir;
    unsigned int kernel_stack;
    flag_t ready;
    unsigned short s_ticks_left;
    unsigned short s_quantum_size;
    time_t cpu_time;
    struct kthread *kthread;
    int ring;
    struct task *next;
} task_t;

extern void init_sched(void);
extern void dump_all_tasks(void);
extern void add_to_queue(task_t *t);
extern task_t *get_task(pid_t pid);
extern task_t *pick_next_task(void);

static task_t *create_task(pid_t pid)
{
    task_t *t = malloc(sizeof(task_t));
    t->pid = pid;
    t->ready = 1;
    t->next = NULL;
    return t;
}

#define NUM_TASKS 1000

int main(void)
{
    task_t *tasks[NUM_TASKS];
    unsigned short i;
    
    init_sched();

    /* Test adding some tasks to the queue */
    assert(get_task(0) == NULL);
    for (i = 0; i < NUM_TASKS; i++) {
        task_t *t = create_task((pid_t)i);
        add_to_queue(t);
        assert(get_task(i) == t);
        tasks[i] = t;
        printf("sched: new task %d\n", i);
    }
    assert(get_task(i) == NULL);
    
    /* Test picking procs */
    for (i = 0; i < NUM_TASKS; i++) {
        assert(pick_next_task() == tasks[i]);
    }
    assert(pick_next_task() == tasks[0]);

    return 0;
}
