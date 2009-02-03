
#ifndef _UTIL_H
#define _UTIL_H

/* Utility functions for the kernel */

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

/* stub()
 *  Print stub information for a function name given if DEBUG.
 */
void stub(char *fname);

/* print_startup()
 *  Output kernel information.
 */
void print_startup(void);

#endif

