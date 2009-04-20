#include "../config.h"
#include <ulysses/kprintf.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>

struct queue {
    task_t *head;
    task_t *tail;
};
struct queue tasks_queue;

static void add_as_head(task_t *t)
{
    tasks_queue.head = t;
    tasks_queue.tail = NULL;
    t->next = NULL;

#if SCHED_DEBUG
    kprintf("add_as_head(): pid %d\n", t->pid);
#endif
}

static void add_as_tail(task_t *t)
{
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

#if SCHED_DEBUG
        kprintf("add_as_tail(): pid %d\n", t->pid);
#endif
}

static void move_to_tail(task_t *t)
{
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
#if SCHED_DEBUG
    kprintf("move_to_tail(): pid %d removed from head\n", t->pid);
    kprintf("move_to_tail(): new head pid %d\n", tasks_queue.head->pid);
#endif
    add_as_tail(t);
}

void init_sched(void)
{
    tasks_queue.head = NULL;
    tasks_queue.tail = NULL;
}

void add_to_queue(task_t *t)
{
    if (tasks_queue.head == NULL) {
        add_as_head(t);
    } else {
        add_as_tail(t);
    }
}

task_t *pick_next_task(void)
{
    task_t *t = tasks_queue.head;

    while (t != NULL) {
        if (t->ready) {
#if SCHED_DEBUG
            kprintf("pick_next_task(): pid %d ready\n", t->pid);
            dump_all_tasks();
#endif
            move_to_tail(t);
            return t;
        }
        t = t->next;
    }

    return NULL;
}

void dump_all_tasks(void)
{
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d ", t->pid);
        t = t->next;
    }
    kprintf("\n");
}
