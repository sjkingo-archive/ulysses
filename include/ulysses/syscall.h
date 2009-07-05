#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

#include <ulysses/callnr.h>
#include <ulysses/isr.h>

#include <sys/types.h>

/* Macros to make the actual system calls with different number of args */
#define syscall0(num) \
({ \
    int __r; \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num)); \
    __r; \
})

#define syscall1(num, arg1) \
({ \
    int __r; \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" ((int)arg1)); \
    __r; \
})

#define syscall2(num, arg1, arg2) \
({ \
    int __r; \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" ((int)arg1), "c" ((int)arg2)); \
    __r; \
})

#define syscall3(num, arg1, arg2, arg3) \
({ \
    int __r; \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" (arg1), "c" (arg2), "d" (arg3)); \
    __r; \
})

/* An entry in the system call table */
struct syscall_entry {
    unsigned int num;
    void *func_addr;
};

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t *regs);

/* Declarations for the actual system calls */

/* sys_dummy()
 *  Move along, nothing to see here.
 */
int sys_dummy(void);

/* sys_exit()
 *  Terminates the calling task "immediately". In reality this will only 
 *  performs an immediate context switch, and the task is destroyed on the 
 *  next time it appears as the head in the scheduling queue.
 */
int sys_exit(void);

/* sys_fork()
 *  Fork the calling process.
 */
int sys_fork(void);

/* sys_read(fd, buf, count)
 *  Read up to count bytes from file descriptor fd into the buffer pointed to
 *  by buf. Return the number of bytes actually read (which may be less than
 *  count).
 */
int sys_read(int fd, char *buf, size_t count);

/* sys_write(fd, buf, count)
 *  Write up to count bytes from the buffer buf to the file descriptor fd.
 *  Return the number of bytes actually written (which may be less than count
 *  if the buffer was smaller).
 */
int sys_write(int fd, const char *buf, size_t count);

/* Reserved for future use */
int sys_open(void);
int sys_close(void);
int sys_waitpid(int pid, int *status, int options);
int sys_execve(const char *filename, char *const argv[], char *const envp[]);

/* sys_getpid()
 *  Returns the process id of the calling process.
 */
int sys_getpid(void);

/* sys_getuid()
 *  Returns the user id of the calling process.
 */
int sys_getuid(void);

#endif
