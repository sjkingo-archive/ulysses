#ifndef _TASK_H
#define _TASK_H

#include <ulysses/kthread.h>
#include <ulysses/paging.h>

#define KERNEL_STACK_SIZE 8192

/* An entry in the task table. */
typedef struct task {
    pid_t pid; /* process id */
    pid_t ppid; /* parent's process id */
    uid_t uid; /* user id */
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    char *name; /* NULL-terminated process name */

    unsigned int esp; /* current stack pointer */
    unsigned int ebp; /* base stack pointer */
    unsigned int eip; /* current instruction pointer */
    page_dir_t *page_dir;
    unsigned int kernel_stack; /* location of kernel stack */

    flag_t ready; /* whether this process is ready to be run */
    unsigned short s_ticks_left; /* number of scheduling ticks left */
    unsigned short s_quantum_size; /* quantum size in ticks */

    struct kthread *kthread; /* NULL if not a kernel thread */

    struct task *next; /* next proc in queue, or NULL if tail */
} task_t;

#define SCHED_QUANTUM 50 /* default quantum for processes */

void init_task(void);

task_t *new_task(char *name);

pid_t fork(void);

pid_t getpid(void);

void check_current_task(void);

void switch_kernel_stack(void);

#endif
