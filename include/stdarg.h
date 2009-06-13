#ifndef _STDARG_H
#define _STDARG_H

/* This file is taken from Minix's stdarg.h, which was taken from 
 * Ken Thompson's stdarg.h:
 *	"You are not expected to understand this"
 */

typedef char *va_list;

#define __vasz(x)		((sizeof(x)+sizeof(int)-1) & ~(sizeof(int) -1))

#define va_start(ap, parmN)	((ap) = (va_list)&parmN + __vasz(parmN))
#define va_arg(ap, type)      \
  (*((type *)((va_list)((ap) = (void *)((va_list)(ap) + __vasz(type))) \
						    - __vasz(type))))
#define va_end(ap)

/* vsprintf()
 *  Format the format string given into a buffer given by buf.
 *  Return the number of characters actually written.
 */
int vsprintf(char *buf, const char *fmt, va_list args);

#endif

