#include <ulysses/kprintf.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/shutdown.h>

void sys_dummy(void)
{
    TRACE_ONCE;
    kprintf("sys_dummy(): Dummy syscall; why was this called?\n");
}

void sys_exit(void)
{
    TRACE_ONCE;
    task_exit();
}

void sys_shutdown(void)
{
    TRACE_ONCE;
    shutdown();
}

void sys_write(const char *buf)
{
    TRACE_ONCE;
    kprintf("%s", buf);
}
