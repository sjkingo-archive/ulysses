#include <ulysses/kprintf.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/shutdown.h>
#include <ulysses/vt.h>

#include <string.h>
#include <sys/types.h>

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
    return -1;
}

int sys_write(int fd, const char *buf, size_t count)
{
    TRACE_ONCE;
    unsigned int i, len;

    if (fd > 2) {
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
