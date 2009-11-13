#ifndef _ULYSSES_UTIL_H
#define _ULYSSES_UTIL_H

/* Utility functions for the kernel that don't really belong anywhere else. */

#include <ulysses/util_x86.h>

#include <sys/types.h>

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

/* parse_cmdline()
 *  Expand the given kernel command line into the struct of kernel
 *  flags (old config.h preprocessor).
 */
void parse_cmdline(const char *cmdline);

/* do_msuptime()
 *  Return the kernel's current uptime in milliseconds.
 */
mseconds_t do_msuptime(void);

#endif
