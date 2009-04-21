#ifndef _SYSCALL_H
#define _SYSCALL_H

void syscall_handler(registers_t regs);

/* The actual system calls */
void do_dummy(void); /* 0 */

#endif
