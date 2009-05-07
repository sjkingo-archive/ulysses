#include <ulysses/gdt.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/shell.h>
#include <ulysses/shell_cmds.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <stdlib.h>

/* shell.c; needed for cmd_history */
extern char *last_cmds[];

/* sched.c; needed for cmd_ps */
extern struct queue tasks_queue;

static void cmd_test(char **args)
{
    TRACE_ONCE;
    kprintf("test with args %s\n", *args);
}

static void cmd_uptime(void)
{
    TRACE_ONCE;
    kprintf("up %d seconds\n", kern.current_time_offset.tv_sec);
}

static void cmd_comefrom(void)
{
    TRACE_ONCE;
    kprintf("Oh come on, we all know that Dijkstra was right:\n");
    kprintf("<http://www.cs.utexas.edu/users/EWD/transcriptions/"
            "EWD02xx/EWD215.html>\n");
}

static void cmd_pf(void)
{
    TRACE_ONCE;
    __asm__ __volatile__("jmp 0x0");
}

static void cmd_history(void)
{
    TRACE_ONCE;
    int i;
    for (i = SHELL_MAX_HISTORY; i >= 0; i--) {
        kprintf("%s", last_cmds[i]);
        kprintf("\n");
    }
}

static void cmd_int_80(char **args)
{
    TRACE_ONCE;
    int a, num;
    
    num = strtol(*args, NULL, 10);
    __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num));
}

static void cmd_ps(void)
{
    TRACE_ONCE;
    kprintf("PID\tPPID\tUID\tNAME\n");
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d\t%d\t%d\t", t->pid, t->ppid, t->uid);
        if (t->kthread == NULL) {
            kprintf("%s\n", t->name);
        } else {
            kprintf("[%s]\n", t->name);
        }
        t = t->next;
    }
}

extern struct shell_command cmds[]; /* we're using it before it is declared */
static void cmd_help(void)
{
    TRACE_ONCE;
    unsigned int i = 0;
    while (cmds[i].cmd != NULL) {
        kprintf("%s", cmds[i].cmd);
        if (cmds[i].func_args != NULL) {
            kprintf("*");
        }
        if (cmds[i].desc == NULL) {
            kprintf("\tNo help information available.\n");
        } else {
            kprintf("\t%s\n", cmds[i].desc);
        }
        i++;
    }
    kprintf("\n* = this command takes arguments\n");
}

static void cmd_exit(void)
{
    TRACE_ONCE;
    kthread_exit();
}

/* Make sure to update this or the command won't be called! */
struct shell_command cmds[] = {
    { "test", NULL, &cmd_test, NULL },
    { "int", NULL, &cmd_int_80, "Send an interrupt 0x80 (syscall)." },

    { "ver", &print_startup, NULL, "Display kernel version." },
    { "uptime", &cmd_uptime, NULL, "Output current kernel uptime." },
    { "halt", &shutdown, NULL, "Shut down the operating system." },
    { "COMEFROM", &cmd_comefrom, NULL, NULL },
    { "dump_tasks", &dump_all_tasks, NULL, "Dump scheduling queue information." },
    { "pf", &cmd_pf, NULL, "Trigger a page fault." },
    { "history", &cmd_history, NULL, "Show interpreter history." },
    { "ps", &cmd_ps, NULL, "Display a snapshot of all currently running "
            "processes." },
    { "help", &cmd_help, NULL, "Display this help information." },
    { "exit", &cmd_exit, NULL, "Cause the shell to exit." },
    
    { "init_task()", &init_task, NULL, NULL },
    { "fork()", &fork, NULL, NULL },
    { "switch_to_ring3()", &switch_to_ring3, NULL, NULL },

    { NULL, NULL, NULL, NULL }, /* sentinel entry; don't remove */
};
