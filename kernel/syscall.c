/* System call implementation. The dispatcher for this is in arch/X/syscall.c.
 *
 * To add a new system call:
 *   1. Add a new definition, macro (optional) and function declaration to
 *      <include/syscall.h>. Make sure you pick a new, unused number for the
 *      syscall.
 *   2. Create the function to do the work of the system call in this file.
 *      The template for naming is sys_X, where X is the name of the syscall.
 *   3. Add an entry to the syscall table.
 */

#include <ulysses/kprintf.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/shutdown.h>
#include <ulysses/syscall.h>
#include <ulysses/vt.h>

#include <errno.h>
#include <string.h>
#include <sys/types.h>

/* System call table */
struct syscall_entry syscalls[] = {
    { SYS_DUMMY, &sys_dummy, },
    { SYS_EXIT, &sys_exit, },
    { SYS_SHUTDOWN, &sys_shutdown, },
    { SYS_WRITE, &sys_write, },
    { SYS_READ, &sys_read, },
    { SYS_FORK, &sys_fork, },
    { 0, NULL }, /* sentinel entry; do not remove */
};

/* Number of system calls. We -1 to ignore the sentinel entry. */
unsigned int num_syscalls = (sizeof(syscalls) / 
        sizeof(struct syscall_entry)) - 1;

/* The actual system calls go here */

int sys_dummy(void)
{
    TRACE_ONCE;
    kprintf("sys_dummy(): Dummy syscall; why was this called?\n");
    return 0;
}

int sys_exit(void)
{
    TRACE_ONCE;
    task_exit();
    errno = ESRCH;
    return -1;
}

int sys_shutdown(void)
{
    TRACE_ONCE;
    if (do_getuid() != 0) {
        kprintf("Only root can do that.\n");
        return -1;
    }
    shutdown();
    errno = ESRCH;
    return -1;
}

int sys_write(int fd, const char *buf, size_t count)
{
    TRACE_ONCE;
    unsigned int i, len;

    if (fd > 2) {
        errno = EBADF;
        return -1;
    }
    
    len = strlen(buf);
    for (i = 0; i < count; i++) {
        if (i >= len) {
            break;
        }
        append_char(buf[i], TRUE, FALSE);
    }

    return i;
}

int sys_read(int fd, char *buf, size_t count)
{
    TRACE_ONCE;
    errno = EBADF;
    return -1;
}

int sys_getuid(void)
{
    TRACE_ONCE;
    return do_getuid();
}

int sys_getpid(void)
{
    TRACE_ONCE;
    return do_getpid();
}

int sys_fork(void)
{
    TRACE_ONCE;
    pid_t pid = do_fork();
    if (pid == -1) {
        errno = ECANCELED;
    }
    return pid;
}
