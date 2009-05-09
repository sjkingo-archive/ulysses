#include <ulysses/kprintf.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/shutdown.h>

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
    shutdown();
    return -1;
}

int sys_write(int fd, const char *buf, size_t count)
{
    TRACE_ONCE;
    kprintf("%s", buf);
    return strlen(buf);
}
