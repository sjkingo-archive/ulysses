#include <ulysses/isr.h>
#include <ulysses/kprintf.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

static void sys_dummy(void)
{
    TRACE_ONCE;
    kprintf("sys_dummy(): Dummy syscall; why was this called?\n");
}

static void sys_exit(void)
{
    TRACE_ONCE;
    task_exit();
}

static void sys_shutdown(void)
{
    TRACE_ONCE;
    shutdown();
}

/* End of system calls */

void *syscalls[] = {
    &sys_dummy,
    &sys_exit,
    &sys_shutdown,
};
unsigned int num_syscalls = (sizeof(syscalls) / sizeof(void *));

void syscall_handler(registers_t regs)
{
    TRACE_ONCE;
    void *syscall_loc;
    int ret;

    if (regs.eax >= num_syscalls) {
        kprintf("Invalid syscall with eax %d\n", regs.eax);
        return;
    }
    
    syscall_loc = syscalls[regs.eax];

    /* Since we don't know how many params the syscall function wants, so
     * just push all of them onto the stack and pop them off later.
     */
    __asm__ __volatile__("  \
            push %1;        \
            push %2;        \
            push %3;        \
            push %4;        \
            push %5;        \
            call *%6;       \
            pop %%ebx;      \
            pop %%ebx;      \
            pop %%ebx;      \
            pop %%ebx;      \
            pop %%ebx;"
            : "=a" (ret) : "r" (regs.edi), "r" (regs.esi), "r" (regs.edx),
            "r" (regs.ecx), "r" (regs.ebx), "r" (syscall_loc));
    regs.eax = ret;
}
