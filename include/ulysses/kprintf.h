#ifndef _ULYSSES_KPRINTF_H
#define _ULYSSES_KPRINTF_H

#include <sys/types.h> /* for flag_t */

/* kprintf()
 *  printf() for the kernel. Takes a format string and variable arguments
 *  list, formats the string using given args and specifiers, and appends
 *  the formatted string to the active VT.
 */
int kprintf(const char *fmt, ...)  __attribute__ ((format (printf, 1, 2)));

/* kdebug()
 *  Same as kprintf(), except prepend the location of the callee for
 *  debugging info.
 */
#define kdebug(fmt, ...) do_kdebug(__FILE__, __LINE__, __func__, fmt, \
        ## __VA_ARGS__)
void do_kdebug(const char *file, int line, const char *func, 
        const char *fmt, ...) __attribute__((format (printf, 4, 5)));

/* kwarn()
 *  Same as kprintf(), except prepend the calling function's name.
 */
#define kwarn(fmt, ...) do_kwarn(__func__, fmt, ## __VA_ARGS__)
void do_kwarn(const char *func, const char *fmt, ...)
        __attribute__((format (printf, 2, 3)));

#endif
