#ifndef _ULYSSES_UTIL_H
#define _ULYSSES_UTIL_H

/* Utility functions for the kernel that don't really belong anywhere else. */

#include <ulysses/util_x86.h>

/* start_init()
 *  Forks the callee and starts the init task. The kernel should be the only
 *  task that calls this.
 */
void start_init(void);

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

/* print_startup()
 *  Output kernel information.
 */
void print_startup(void);

/* lock_kernel()
 * unlock_kernel()
 *  Lock the kernel so that code in between these two calls is executed 
 *  atomically with no interruptions.
 */
inline void lock_kernel(void);
inline void unlock_kernel(void);

#endif
