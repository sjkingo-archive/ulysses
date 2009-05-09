#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

/* System call taking 0 arguments */
#define DECL_SYSCALL0(func) int do_##func()
#define DEFN_SYSCALL0(func, num) \
    static void sys_##func(); \
    int do_##func() \
    { \
        int a; \
        __asm__ __volatile__("int $0x80" : "=a" (a) : "0" (num)); \
        return a; \
    }

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t regs);

/* The actual system calls */
DECL_SYSCALL0(dummy); /* 0 */
DECL_SYSCALL0(exit); /* 1 */

#endif
