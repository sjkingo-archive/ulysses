#ifndef _SCHED_H
#define _SCHED_H

#include <ulysses/task.h>

void init_sched(void);

void add_to_queue(task_t *t);

task_t *pick_next_task(void);

void dump_all_tasks(void);

#endif
