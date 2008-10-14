
#ifndef _KPRINTF_H
#define _KPRINTF_H

/* In case someone gets sloppy... */
#define printf kprintf

/* kputc()
 *  Print a single character to screen.
 */
void kputc(const char c);

/* kprintf()
 *  printf() for the kernel. Takes a format string and variable arguments
 *  list, formats the string using given args and specifiers, and appends
 *  the formatted string to the screen.
 */
void kprintf(const char *fmt, ...);

#endif

