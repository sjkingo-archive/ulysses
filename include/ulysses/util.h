#ifndef _ULYSSES_UTIL_H
#define _ULYSSES_UTIL_H

/* Utility functions for the kernel that don't really belong anywhere else. */

#include <ulysses/util_x86.h>

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

/* print_startup()
 *  Output kernel information.
 */
void print_startup(void);

#endif
