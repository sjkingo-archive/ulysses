#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

#define syscall0(num) \
    { \
        int __a; \
        __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num)); \
    }
#define syscall1(num, arg1) \
    { \
        int __a; \
        __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num), "b" ((int)arg1)); \
    }

#define SYS_DUMMY 0
#define SYS_EXIT 1
#define SYS_SHUTDOWN 2
#define SYS_WRITE 3

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t regs);

#endif
