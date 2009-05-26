#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/* Cruff needed for the task_t struct */
typedef void page_dir_t;
typedef int flag_t;
struct kthread { };

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

int main(void)
{
    task_t *tasks[4];
    unsigned short i;
    
    init_sched();

    /* Test adding some tasks to the queue */
    assert(get_task(0) == NULL);
    for (i = 0; i < 4; i++) {
        task_t *t = malloc(sizeof(task_t));
        t->pid = i;
        t->ready = 1;
        t->next = NULL;
        add_to_queue(t);
        assert(get_task(i) == t);
        tasks[i] = t;
    }
    assert(get_task(4) == NULL);
    
    /* Test picking procs */
    assert(pick_next_task() == tasks[0]);
    assert(pick_next_task() == tasks[1]);
    assert(pick_next_task() == tasks[2]);
    assert(pick_next_task() == tasks[3]);
    assert(pick_next_task() == tasks[0]);

    return 0;
}
