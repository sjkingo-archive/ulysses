#ifndef _ULYSSES_CALLNR
#define _ULYSSES_CALLNR

/* This file contains the definitions for each system call number.
 * They (mostly) match the Linux call numbers, so programs built with a Linux
 * libc should have matching system call numbers.
 * 
 * See <ulysses/syscall.h> for more information on the calls themselves.
 */

#define __NR_dummy      0
#define __NR_exit       1
#define __NR_fork       2
#define __NR_read       3
#define __NR_write      4
#define __NR_open       5
#define __NR_close      6
#define __NR_waitpid    7
#define __NR_execve     11
#define __NR_getpid     20
#define __NR_getuid     24

#endif
