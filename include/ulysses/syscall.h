#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

/* Macros to make the actual system calls with different number of args */
#define syscall0(num) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num)); }
#define syscall1(num, arg1) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num), \
            "b" ((int)arg1)); }
#define syscall2(num, arg1, arg2) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num), \
            "b" ((int)arg1), "c" ((int)arg2)); }
#define syscall3(num, arg1, arg2, arg3) \
    { int __a; __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (num), \
            "b" ((int)arg1), "c" ((int)arg2), "d" ((int)arg3)); }

/* syscall_handler()
 *  The interrupt vector for int 80. This looks up the syscall number from
 *  the eax register and dispatches the required system call.
 */
void syscall_handler(registers_t regs);

/* The actual system calls */

/* sys_dummy()
 *  Move along, nothing to see here.
 */
#define SYS_DUMMY 0
int sys_dummy(void);

/* sys_exit()
 *  Terminates the calling task "immediately". In reality this will only 
 *  performs an immediate context switch, and the task is destroyed on the 
 *  next time it appears as the head in the scheduling queue.
 */
#define SYS_EXIT 1
#define exit() syscall0(SYS_EXIT)
int sys_exit(void);

/* sys_shutdown()
 *  Shut down the system now. The kernel may refuse this if the owning user
 *  is not root (uid 0), in which case it will return -1.
 */
#define SYS_SHUTDOWN 2
int sys_shutdown(void);

/* sys_write(fd, buf, count)
 *  Write up to count bytes from the buffer buf to the file descriptor fd.
 *  Return the number of bytes actually written (which may be less than count
 *  if the buffer was smaller).
 */
#define SYS_WRITE 3
#define write(fd, buf, count) syscall3(SYS_WRITE, fd, buf, count)
int sys_write(int fd, const char *buf, size_t count);

/* Reserved */
#define SYS_READ 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

/* sys_getuid()
 *  Returns the user id of the calling process.
 */
#define SYS_GETUID 7
#define getuid() syscall0(SYS_GETUID)
int sys_getuid(void);

/* sys_getpid()
 *  Returns the process id of the calling process.
 */
#define SYS_GETPID 8
#define getpid() syscall0(SYS_GETPID)
int sys_getpid(void);

#endif
