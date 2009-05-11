#ifndef _ULYSSES_SCHED_H
#define _ULYSSES_SCHED_H

/* The kernel scheduler is a simple round-robin scheduler that acts as a
 * basic queue. Tasks are added to the tail and picked from the head.
 */

#include <ulysses/task.h>

/* A scheduling queue */
struct queue {
    task_t *head;
    task_t *tail;
};

/* init_sched()
 *  Set up the scheduler.
 */
void init_sched(void);

/* add_to_queue()
 *  Add the given task to the end of the ready queue.
 */
void add_to_queue(task_t *t);

/* pick_next_task()
 *  Return the task that should be run next.
 */
task_t *pick_next_task(void);

/* dump_all_tasks()
 *  Display information on the ready queue.
 */
void dump_all_tasks(void);

/* get_task()
 *  Return a pointer to the task structure belonging to pid, or NULL if pid
 *  does not exist.
 */
task_t *get_task(pid_t pid);

#endif
