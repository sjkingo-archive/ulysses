#ifndef _BLOOM_ERRNO
#define _BLOOM_ERRNO

/* errno is a positive integer describing the last system call's status. */
extern int errno;

#define EBADF 1 /* Bad file descriptor */
#define EINVAL 2 /* Invalid argument */
#define EPERM 3 /* Operation not permitted */
#define ESRCH 4 /* Not such process */
#define ECANCELED 5 /* Operation cancelled */

#endif
