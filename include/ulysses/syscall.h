#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

/* Macros to make the actual system calls with different number of args */
#define syscall0(num) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num)); }
#define syscall1(num, arg1) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num), "b" ((int)arg1)); }

/* The actual system calls */

#define SYS_DUMMY 0
void sys_dummy(void);

#define SYS_EXIT 1
void sys_exit(void);

#define SYS_SHUTDOWN 2
void sys_shutdown(void);

#define SYS_WRITE 3
void sys_write(const char *buf);

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t regs);

#endif
