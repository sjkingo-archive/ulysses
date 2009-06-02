
#ifndef _UNISTD_H
#define _UNISTD_H

#define _POSIX_VERSION 200112L /* conforms to IEEE 1003.1-2001 */

/* POSIX 2.7.1 says this must be defined here */
#define NULL ((void *)0)

/* POSIX 2.7.1 tests for access(2) */
#define F_OK 0 /* file exists */
#define R_OK 1 /* readable */
#define W_OK 2 /* writeable */
#define X_OK 3 /* executable */

/* Default file descriptors */
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* sleep()
 *  Sleep for (at least) the given number of microseconds, but no less than.
 *  This is not always accurate, as it relies on the timer being of high
 *  enough precision to measure small units of microseconds.
 */
inline void sleep(unsigned long msecs);

#endif

