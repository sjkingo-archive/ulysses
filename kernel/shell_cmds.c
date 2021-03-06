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

#include <ulysses/callnr.h>
#include <ulysses/cputest.h>
#include <ulysses/exec.h>
#include <ulysses/drivers/disk.h>
#include <ulysses/fs.h>
#include <ulysses/gdt.h>
#include <ulysses/initrd.h>
#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/module.h>
#include <ulysses/shell.h>
#include <ulysses/shell_cmds.h>
#include <ulysses/sched.h>
#include <ulysses/shutdown.h>
#include <ulysses/stats.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>
#include <ulysses/util_x86.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* shell.c; needed for cmd_history */
extern char *last_cmds[];

/* sched.c; needed for cmd_ps */
extern struct queue tasks_queue;

static void cmd_uptime(void)
{
    kprintf("up %ld seconds (%ld milliseconds)\n", 
            kern.current_time_offset.tv_sec, 
            kern.current_time_offset.tv_msec);
}

static void cmd_comefrom(void)
{
    kprintf("Oh come on, we all know that Dijkstra was right:\n");
    kprintf("<http://www.cs.utexas.edu/users/EWD/transcriptions/"
            "EWD02xx/EWD215.html>\n");
}

static void cmd_pf(void)
{
    PF;
}

static void cmd_history(void)
{
    int i;
    for (i = SHELL_MAX_HISTORY; i >= 0; i--) {
        kprintf("%s", last_cmds[i]);
        kprintf("\n");
    }
}

static void cmd_int_80(char **args)
{
    int a, num;
    
    num = strtol(args[0], NULL, 10);
    a = INT80(num);
}

static void cmd_kexec(char **args)
{
    kexec(args[0]);
}

static void cmd_ps(void)
{
    kprintf("PID\tPPID\tUID\tCPU\tVMEM\tSTATE\tNAME\n");
    task_t *t = tasks_queue.head;
    while (t != NULL) {
        kprintf("%d\t%d\t%d\t%ld\t%ld\t", t->pid, t->ppid, t->uid, 
                stats_get_cpu_time(t->pid), stats_get_virtual_mem(t->pid));
        if (t->kthread == NULL) {
            kprintf("-\t%s", t->name);
        } else {
            kprintf("%d\t[%s]", t->kthread->state, t->name);
        }
        kprintf(" (%d)\n", t->ring);
        t = t->next;
    }
}

extern struct shell_command cmds[]; /* we're using it before it is declared */
static void cmd_help(void)
{
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
    kprintf("Shell is exiting; press F8 to start a new one.\n");
    kthread_exit();
}

static void kt_test(void)
{
    unsigned short i;
    for (i = 0; i < 5; i++) {
        kprintf("Hello from pid %d: i %d\n", do_getpid(), i);
    }
}

static void cmd_kt_test(void)
{
    unsigned short i;
    for (i = 0; i < 3; i++) {
        kthread_create(kt_test, "kt_test");
    }
}

static void kt_bomb(void)
{
    kprintf("kthread %d will yield once and exit\n", do_getpid());
    kthread_yield();
    kthread_exit();
}

static void cmd_kt_bomb(void)
{
    unsigned long long i = 0;
    while (1) {
        pid_t pid = kthread_create(kt_bomb, "bomb");
        kprintf("Spawned kthread %Ld with pid %d\n", i, pid);
        kthread_yield();
        i++;
    }
}

static void cmd_errno(void)
{
    kprintf("%d\n", errno);
}

static void cmd_kill(char **args)
{
    pid_t pid = (pid_t)strtol(args[0], NULL, 10);
    kill_task(pid);
}

static void cmd_f00f(void)
{
    test_f00f();
    kprintf("This CPU doesn't appear to have the f00f bug. Yay!\n");
}

static void cmd_cat(char **args)
{
    
    struct file *f = load_file(args[0]);
    if (f == NULL) {
        kprintf("cat: %s: No such file\n", args[0]);
    } else {
        ((char *)f->data)[f->size++] = '\0';
        kprintf("%s", (char *)f->data);
    }
}

static void kt_ring3(void)
{
    char *msg = "Hello via SYS_WRITE in usermode; now going into a busy "
            "loop to test preemption.";
    unsigned int len = strlen(msg);

    switch_to_ring3();
    syscall0(__NR_dummy);
    syscall3(__NR_write, STDOUT_FILENO, msg, len);
    while (1); /* spin a bit */
}

static void cmd_ring3(void)
{
    kthread_create(kt_ring3, "ring3");
}

static void cmd_flag(char **args)
{
    flag_t a;

    if (args[0] == NULL || args[1] == NULL) {
        kprintf("Usage: flag enable|disable sched|ticks|task|interrupt\n");
        return;
    }

    if (strcmp(args[0], "enable") == 0) {
        a = TRUE;
    } else if (strcmp(args[0], "disable") == 0) {
        a = FALSE;
    } else {
        kprintf("Unknown action %s\n", args[0]);
        return;
    }

    if (strcmp(args[1], "sched") == 0) {
        kern.flags.debug_sched = a;
    } else if (strcmp(args[1], "ticks") == 0) {
        kern.flags.debug_ticks = a;
    } else if (strcmp(args[1], "task") == 0) {
        kern.flags.debug_task = a;
    } else if (strcmp(args[1], "interrupt") == 0) {
        kern.flags.debug_interrupt = a;
    } else {
        kprintf("Unknown flag %s\n", args[1]);
        return;
    }
}

static void cmd_echo(char **args)
{
    while (*args != NULL) {
        kprintf("%s ", *args);
        args++;
    }
    kprintf("\n");
}

void cmd_fork(void)
{
    pid_t pid = do_fork();
    if (pid == -1) {
        kprintf("do_fork() failed and returned -1: check errno\n");
    } else if (pid == 0) {
        kprintf("Hello from child, exiting...\n");
        task_exit();
    } else {
        kprintf("Hello from parent, the child's pid is %d\n", pid);
    }
}

static void cmd_kmalloc(void)
{
    int i;
    int iter = 10000;
    int size = 10;
    for (i = 0; i < iter; i++) {
        kprintf("kmalloc %d returned %p\n", i, kmalloc(size));
    }
    kprintf("%d bytes allocated\n", iter*size);
}

static void cmd_init(void)
{
    create_init();
}

static void cmd_lmod(char **args)
{
    if (args[0] == NULL) {
        return;
    }
    load_module(args[0]);
}

static void cmd_ksyms(void)
{
    dump_syms();
}

static void cmd_mount(char **args)
{
    unsigned short disk;

    if (args[0] == NULL || args[1] == NULL || args[3] != NULL) {
        kprintf("Usage: mount ext2 disk [reserved]\n");
        return;
    }

    disk = strtol(args[1], NULL, 10);
    mount_fs(disk, args[0]);
}

/* Make sure to update this or the command won't be called! */
struct shell_command cmds[] = {
    /* Commands that take arguments */
    { "int", NULL, &cmd_int_80, "Send an interrupt 0x80 (syscall)." },
    { "kill", NULL, &cmd_kill, "Kill a task matching the given process id." },
    { "cat", NULL, &cmd_cat, "Concatenate the contents of the given filename "
            "to screen." },
    { "flag", NULL, &cmd_flag, "Change kernel flag given by second argument." },
    { "echo", NULL, &cmd_echo, "Display a line of text given by the arguments." },
    { "lmod", NULL, &cmd_lmod, "Load a kernel module." },
    { "kexec", NULL, &cmd_kexec, "Execute a process in the kernel." },
    { "mount", NULL, &cmd_mount, "Mount a file system." },

    /* Commands that take no arguments */
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
    { "kt_bomb", &cmd_kt_bomb, NULL, "Spawn kernel threads until the OS "
            "crashes." },
    { "errno", &cmd_errno, NULL, "Output the value of errno." },
    { "f00f", &cmd_f00f, NULL, "The Four Bytes Of The Apocalypse." },
    { "ring3", &cmd_ring3, NULL, "Run a test in user mode." },
    { "fork", &cmd_fork, NULL, "Fork the shell. This shouldn't work!" },
    { "kmalloc", &cmd_kmalloc, NULL, "Run a kmalloc test." },
    { "init", &cmd_init, NULL, "Set up the init process." },
    { "ksyms", &cmd_ksyms, NULL, "Dump the kernel symbols and their "
            "addresses." },
    { "lsata", &dump_drives, NULL, "Dump information on detected ata drives." },
    
    { NULL, NULL, NULL, NULL }, /* sentinel entry; don't remove */
};
