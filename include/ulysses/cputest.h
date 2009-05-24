#ifndef _ULYSSES_CPUTEST_H
#define _ULYSSES_CPUTEST_H

/* test_f00f()
 *  Attempts to trigger the f00f bug present on Intel processors before
 *  the Pentium 2. If the bug exists, it will freeze the CPU and this 
 *  function will never return. If the function returns, the bug does
 *  not exist.
 */
void test_f00f(void);

#endif
