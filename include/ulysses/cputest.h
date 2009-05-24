#ifndef _ULYSSES_CPUTEST_H
#define _ULYSSES_CPUTEST_H

#include <sys/types.h>

/* test_f00f()
 *  Attempts to trigger the f00f bug present on Intel processors before
 *  the Pentium 2. If the bug exists, it will freeze the CPU and this 
 *  function will never return. If the function returns, the bug does
 *  not exist.
 */
void test_f00f(void);

/* test_fdiv()
 *  Checks if the FPU attached to the CPU has the Pentium FDIV bug.
 *  Returns TRUE if the bug exists in the FPU, FALSE otherwise.
 *  More info: http://en.wikipedia.org/wiki/Pentium_FDIV_bug
 */
flag_t test_fdiv(void);

#endif
