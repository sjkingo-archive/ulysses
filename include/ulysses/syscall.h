#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t regs);

/* The actual system calls */
void sys_dummy(void); /* 0 */

#endif
