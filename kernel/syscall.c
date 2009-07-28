/* kernel/syscall.c - system call layer
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
 *
 *
 * The dispatcher for this is in arch/X/syscall.c.
 * To add a new system call:
 *   1. Add a new definition to <include/ulysses/callnr.h> and function
 *      declaration to <include/ulysses/syscall.h>.  Make sure you pick a new,
 *      unused number for the syscall.
 *   2. Create the function to do the work of the system call in this file.
 *      The template for naming is sys_X, where X is the name of the syscall.
 *   3. Add an entry to the syscall table.
 */

#include <ulysses/callnr.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/task.h>
#include <ulysses/shutdown.h>
#include <ulysses/syscall.h>
#include <ulysses/vt.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>

/* System call table */
struct syscall_entry syscalls[] = {
    { __NR_dummy, &sys_dummy, },
    { __NR_exit, &sys_exit, },
    { __NR_fork, &sys_fork, },
    { __NR_read, &sys_read, },
    { __NR_write, &sys_write, },
    { __NR_open, &sys_open, },
    { __NR_close, &sys_close, },
    { __NR_waitpid, &sys_waitpid, },
    { __NR_execve, &sys_execve, },
    { __NR_getpid, &sys_getpid, },
    { __NR_getuid, &sys_getuid, },
    { 0, NULL }, /* sentinel entry; do not remove */
};

/* Number of system calls. We -1 to ignore the sentinel entry. */
unsigned int num_syscalls = (sizeof(syscalls) / 
        sizeof(struct syscall_entry)) - 1;

/* The actual system calls go here */

int sys_dummy(void)
{
    kprintf("sys_dummy(): Dummy syscall; why was this called?\n");
    return 0;
}

int sys_exit(void)
{
    task_exit();
    errno = ESRCH;
    return -1;
}

int sys_fork(void)
{
    return do_fork();
}

int sys_read(int fd, char *buf, size_t count)
{
    errno = EBADF;
    return -1;
}

int sys_write(int fd, const char *buf, size_t count)
{
    unsigned int i, len;

    if (fd > 2) {
        errno = EBADF;
        return -1;
    }

    len = strlen(buf);
    if (len < count) {
        count = len;
    }

    for (i = 0; i < count; i++) {
        kprintf("%c", buf[i]);
    }

    return i;
}

int sys_open(void)
{
    return -1;
}

int sys_close(void)
{
    return -1;
}

int sys_waitpid(int pid, int *status, int options)
{
    return -1;
}

int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
    return do_execv(filename, argv);
}

int sys_getpid(void)
{
    return do_getpid();
}

int sys_getuid(void)
{
    return do_getuid();
}
