/* kernel/shell_cmds.c - commands for the in-kernel shell (see shell.c)
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

#include <ulysses/cputest.h>
#include <ulysses/gdt.h>
#include <ulysses/initrd.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/shell.h>
#include <ulysses/shell_cmds.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <errno.h>
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
    kprintf("up %d seconds (%d milliseconds)\n", 
            kern.current_time_offset.tv_sec, 
            kern.current_time_offset.tv_msec);
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
    kprintf("PID\tPPID\tUID\tCPU\tSTATE\tNAME\n");
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d\t%d\t%d\t%d\t", t->pid, t->ppid, t->uid, t->cpu_time);
        if (t->kthread == NULL) {
            kprintf("-\t%s", t->name);
        } else {
            kprintf("%d\t[%s]", t->kthread->state, t->name);
        }
        kprintf(" (%d)\n", t->ring);
        t = t->next;
    }
}

static void cmd_sched(void)
{
    TRACE_ONCE;
    kprintf("CPU (s)\tNAME\n");
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d\t%s\n", t->cpu_time, t->name);
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
    kprintf("Shell is exiting; press F8 to start a new one.\n");
    kthread_exit();
}

static void kt_test(void)
{
    TRACE_ONCE;
    unsigned short i;
    for (i = 0; i < 5; i++) {
        kprintf("Hello from pid %d: i %d\n", do_getpid(), i);
    }
}

static void cmd_kt_test(void)
{
    TRACE_ONCE;
    unsigned short i;
    for (i = 0; i < 3; i++) {
        kthread_create(kt_test, "kt_test");
    }
}

static void cmd_dummy(void)
{
    TRACE_ONCE;
    syscall0(SYS_DUMMY);
}

static void kt_pf(void)
{
    TRACE_ONCE;
    __asm__ __volatile__("jmp 0x0");
}

static void cmd_kt_pf(void)
{
    TRACE_ONCE;
    unsigned short i;
    for (i = 0; i < 3; i++) {
        kthread_create(kt_pf, "kt_pf");
    }
}

static void kt_bomb(void)
{
    while (1) kthread_yield();
}

static void cmd_kt_bomb(void)
{
    TRACE_ONCE;
    unsigned long long i = 0;
    while (1) {
        pid_t pid = kthread_create(kt_bomb, "bomb");
        kprintf("Spawned kthread %d with pid %d\n", i, pid);
        kthread_yield();
        i++;
    }
}

static void cmd_errno(void)
{
    TRACE_ONCE;
    kprintf("%d\n", errno);
}

static void cmd_kill(char **args)
{
    TRACE_ONCE;
    pid_t pid = (pid_t)strtol(*args, NULL, 10);
    kill_task(pid);
}

static void cmd_f00f(void)
{
    TRACE_ONCE;
    test_f00f();
    kprintf("This CPU doesn't appear to have the f00f bug. Yay!\n");
}

static void cmd_cat(char **args)
{
    TRACE_ONCE;
    
    struct file *f = load_file(args[0]);
    if (f == NULL) {
        kprintf("cat: %s: No such file\n", args[0]);
    } else {
        ((char *)f->data)[f->size++] = '\0';
        kprintf("%s", (char *)f->data);
    }
}

/* Make sure to update this or the command won't be called! */
struct shell_command cmds[] = {
    { "test", NULL, &cmd_test, NULL },
    { "int", NULL, &cmd_int_80, "Send an interrupt 0x80 (syscall)." },
    { "kill", NULL, &cmd_kill, "Kill a task matching the given process id." },
    { "cat", NULL, &cmd_cat, "Concatenate the given file to screen." },

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
    { "kt_test", &cmd_kt_test, NULL, "Run a kthreads test." },
    { "kt_pf", &cmd_kt_pf, NULL, "Run a kthreads test where each thread will "
            "trigger a page fault." },
    { "kt_bomb", &cmd_kt_bomb, NULL, "Spawn kernel threads until the OS "
            "crashes." },
    { "dummy", &cmd_dummy, NULL, "Send a dummy system call." },
    { "errno", &cmd_errno, NULL, "Output the value of errno." },
    { "f00f", &cmd_f00f, NULL, "The Four Bytes Of The Apocalypse." },
    { "sched", &cmd_sched, NULL, "Display scheduling information." },
    
    { "init_task()", &init_task, NULL, NULL },
    { "fork()", &do_fork, NULL, NULL },
    { "switch_to_ring3()", &switch_to_ring3, NULL, NULL },

    { NULL, NULL, NULL, NULL }, /* sentinel entry; don't remove */
};
