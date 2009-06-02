#ifndef _ULYSSES_SYSCALL_H
#define _ULYSSES_SYSCALL_H

#include <sys/types.h>

/* Macros to make the actual system calls with different number of args */
#define syscall0(num) \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num));
#define syscall1(num, arg1) \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" ((int)arg1));
#define syscall2(num, arg1, arg2) \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" ((int)arg1), "c" ((int)arg2));
#define syscall3(num, arg1, arg2, arg3) \
    __asm__ __volatile__("int $0x80" : "=a" (__r) : "0" (num), \
            "b" (arg1), "c" (arg2), "d" (arg3));

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
#define SYS_DUMMY 0
int sys_dummy(void);
static inline void dummy(void)
{
    int __r;
    syscall0(SYS_DUMMY);
}

/* sys_exit()
 *  Terminates the calling task "immediately". In reality this will only 
 *  performs an immediate context switch, and the task is destroyed on the 
 *  next time it appears as the head in the scheduling queue.
 */
#define SYS_EXIT 1
int sys_exit(void);
static inline void exit(void)
{
    int __r;
    syscall0(SYS_EXIT);
}

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
int sys_write(int fd, const char *buf, size_t count);
static inline ssize_t write(int fd, const char *buf, size_t bytes)
{
    int __r;
    syscall3(SYS_WRITE, fd, (int)buf, bytes);
    return (ssize_t)__r;
}

/* sys_read(fd, buf, count)
 *  Read up to count bytes from file descriptor fd into the buffer pointed to
 *  by buf. Return the number of bytes actually read (which may be less than
 *  count).
 */
#define SYS_READ 4
int sys_read(int fd, char *buf, size_t count);
static inline ssize_t read(int fd, char *buf, size_t count)
{
    int __r;
    syscall3(SYS_READ, fd, (int)buf, count);
    return (ssize_t)__r;
}

/* Reserved for future use */
#define SYS_OPEN 5
int sys_open(void);
#define SYS_CLOSE 6
int sys_close(void);

/* sys_getuid()
 *  Returns the user id of the calling process.
 */
#define SYS_GETUID 7
int sys_getuid(void);
static inline uid_t getuid(void)
{
    int __r;
    syscall0(SYS_GETUID);
    return (uid_t)__r;
}

/* sys_getpid()
 *  Returns the process id of the calling process.
 */
#define SYS_GETPID 8
int sys_getpid(void);
static inline pid_t getpid(void)
{
    int __r;
    syscall0(SYS_GETPID);
    return (pid_t)__r;
}

/* sys_fork()
 *  Fork the calling process.
 */
#define SYS_FORK 9
int sys_fork(void);
static inline pid_t fork(void)
{
    int __r;
    syscall0(SYS_FORK);
    return (pid_t)__r;
}

#endif
