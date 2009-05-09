#include <ulysses/isr.h>
#include <ulysses/kprintf.h>
#include <ulysses/syscall.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/shutdown.h>

extern struct syscall_entry syscalls[];
extern unsigned int num_syscalls;

void syscall_handler(registers_t regs)
{
    TRACE_ONCE;
    struct syscall_entry sc;
    int ret;

    /* Find a matching system call in the table */
    unsigned int i = 0;
    while (syscalls[i].func_addr != NULL) {
        if (syscalls[i].num == regs.eax) {
            sc = syscalls[i];
            break;
        }
        i++;
    }

    /* Make sure we've found a valid system call */
    if (i >= num_syscalls) {
        kprintf("Invalid system call %d\n", regs.eax);
        return;
    }

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
            "r" (regs.ecx), "r" (regs.ebx), "r" (sc.func_addr));

    /* Place the return value in eax so we can pass it back to usermode */
    regs.eax = ret;
}
