#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/shell.h>
#include <ulysses/shell_cmds.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/util.h>

/* shell.c; needed for cmd_history */
extern char *last_cmds[];

static void cmd_test(char **args)
{
    kprintf("test with args %s\n", *args);
}

static void cmd_uptime(void)
{
    kprintf("up %d seconds\n", kern.current_time_offset.tv_sec);
}

static void cmd_comefrom(void)
{
    kprintf("Oh come on, we all know that Dijkstra was right:\n");
    kprintf("<http://www.cs.utexas.edu/users/EWD/transcriptions/"
            "EWD02xx/EWD215.html>\n");
}

static void cmd_exit(void)
{
#if KERN_INTERACTIVE
    startup_kernel();
#else
    shutdown();
#endif
}

static void cmd_pf(void)
{
    __asm__ __volatile__("jmp 0x0");
}

static void cmd_history(void)
{
    int i;
    for (i = SHELL_MAX_HISTORY; i >= 0; i--) {
        kprintf("%s", last_cmds[i]);
        kprintf("\n");
    }
}

/* Make sure to update this or the command won't be called! */
struct shell_command cmds[] = {
    { "test", NULL, &cmd_test },
    { "ver", &print_startup, NULL },
    { "uptime", &cmd_uptime, NULL },
    { "halt", &shutdown, NULL },
    { "COMEFROM", &cmd_comefrom, NULL },
    { "exit", &cmd_exit, NULL },
    { "dump_tasks", &dump_all_tasks, NULL },
    { "pf", &cmd_pf, NULL },
    { "history", &cmd_history, NULL },
    
    { "startup_kernel()", &startup_kernel, NULL },
    { "init_task()", &init_task, NULL },
    { "fork()", &fork, NULL },

    { NULL, NULL, NULL }, /* sentinel entry; don't remove */
};
