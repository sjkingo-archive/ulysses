#ifndef _ULYSSES_TASK_H
#define _ULYSSES_TASK_H

/* This is the task manager (tasker). It is responsible for managing all
 * tasks (kernel threads and user processes) on the system.
 *
 * It does not, however, manage *when* these tasks will be executed -- the
 * scheduler is responsible for that.
 */

#include <ulysses/kthread.h>
#include <ulysses/paging.h>

#include <sys/types.h>

#define KERNEL_STACK_SIZE 8192

/* An entry in the task table. */
typedef struct task {
    pid_t pid; /* process id */
    pid_t ppid; /* parent's process id */
    uid_t uid; /* user id */
    gid_t egid; /* effective group id */
    gid_t rgid; /* real group id */

    char *name; /* NULL-terminated process name */
    int ring;

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

/* init_task()
 *  Set up the task manager. This creates a hand-crafted kernel process 
 *  (pid 0) that provides a sane environment for new_task() and fork() to 
 *  clone off.
 */
void init_task(void);

/* new_task()
 *  Create a new task with the given name and return its task structure.
 *  This creates a default stack for the task, but does not call the scheduler
 *  to manage the task.
 */
task_t *new_task(const char *name);

/* free_task()
 *  Destroy the given task's data structures. Typically called by the
 *  scheduler to remove a task.
 */
void free_task(task_t *task);

/* task_exit()
 *  Flag the current task to be destroyed and trigger a context switch. This 
 *  does not actually remove the task: the scheduler will do this next time 
 *  it goes to be picked.
 *
 *  This function is set as the bottom pointer on each task's stack so it can 
 *  be destroyed on return or exit.
 */
void task_exit(void);

/* kill_task()
 *  Flag the task with the given pid to be destroyed.
 */
void kill_task(pid_t pid);

/* kill_all_tasks()
 *  Flags all tasks on the system to be destroyed, except for the kernel.
 *  This would typically only be done by the kernel at shutdown.
 */
void kill_all_tasks(void);

/* do_fork()
 *  Clone the current task and run it. Returns 0 to the child and the child's
 *  pid to the parent.
 */
pid_t do_fork(void);

/* do_getpid()
 *  Return the pid of the currently executing task.
 */
pid_t do_getpid(void);

/* do_getuid()
 *  Return the user id of the currently executing task.
 */
uid_t do_getuid(void);

/* check_current_task()
 *  Check the current task's quantum and preempt it if needed.
 */
void check_current_task(void);

/* switch_kernel_stack()
 *  Change the current stack when switching processes.
 */
void switch_kernel_stack(void);

/* change_current_task()
 *  Trigger a forced context switch to another task, without first checking
 *  the task's quantum.
 */
void change_current_task(void);

/* set_current_ring3()
 *  Sets the current task to ring 3. This simply updates the value in the
 *  task struct for use by scheduler et al.
 */
void set_current_ring3(void);

/* update_cpu_time()
 *  Updates the time the current task has spent on the CPU.
 */
void update_cpu_time(void);

/* stats_vmem_add()
 *  Collect stats on this process' virtual memory usage.
 */
void stats_vmem_add(unsigned int bytes);

/* change_name()
 *  Change the name of the callee task to that given by new_name.
 */
void change_name(const char *new_name);

/* get_name()
 *  Returns the name of the calling task.
 */
char *get_name(void);

#endif
