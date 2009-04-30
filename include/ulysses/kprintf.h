#ifndef _ULYSSES_KPRINTF_H
#define _ULYSSES_KPRINTF_H

#include <sys/types.h> /* for flag_t */

/* kprintf()
 *  printf() for the kernel. Takes a format string and variable arguments
 *  list, formats the string using given args and specifiers, and appends
 *  the formatted string to the active VT.
 * kprintf_all()
 *  As above, but print to all VTs.
 */
void kprintf(const char *fmt, ...);
void kprintf_all(const char *fmt, ...);

#endif
