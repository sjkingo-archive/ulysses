#ifndef _ULYSSES_KPRINTF_H
#define _ULYSSES_KPRINTF_H

#include <sys/types.h> /* for flag_t */

/* kprintf()
 *  printf() for the kernel. Takes a format string and variable arguments
 *  list, formats the string using given args and specifiers, and appends
 *  the formatted string to the active VT.
 */
int kprintf(const char *fmt, ...)  __attribute__ ((format (printf, 1, 2)));

#endif
