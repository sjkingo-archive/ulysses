
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

#include <stdarg.h>
#include <stdlib.h>

static void kputc(const char c, flag_t all)
{
    append_char(c, TRUE, all);
}

static void kputs(const char *str, flag_t all)
{
    const char *ptr; /* since we may want to change what str is pointing to */
    if (str == NULL) ptr = "(null)";
    else ptr = str;

    while (*ptr != '\0') {
        kputc(*ptr, all);
        ptr++;
    }
}

static void kputd(const int d, flag_t all)
{
    kputs(itoa(d), all);
}

static void kputdu(const unsigned int ud, flag_t all)
{
    kputs(itoa(ud), all); /* XXX itoa() takes a signed int! */
}

static void kputx(const unsigned long hex, const char hex_table[16], 
        flag_t all)
{
    unsigned long h = hex; /* since we are modifying it */
    char *str;

    do {
        *--str = hex_table[(h % 0x10)];
    } while ((h /= 0x10) > 0);

    kputs(str, all);
}

/* kprint()
 *  This formats and puts individual chars from the format string fmt.
 *  Thank you Ken Thompson for giving us this >_<
 */
static void kprint(const char *fmt, va_list argp, flag_t all)
{
    while (*fmt != '\0') {
        /* Format specifier, read ahead and format the arg into a string */
        if (*fmt == '%') {
            fmt++; /* skip the % since it's no longer important */
            switch (*fmt) {
                case 's':
                    kputs(va_arg(argp, char *), all);
                    break;

                case 'c':
                    kputc(va_arg(argp, char), all);
                    break;

                case 'u': /* unsigned int */
                    kputdu(va_arg(argp, unsigned int), all);
                    break;

                case 'i': /* signed int */
                case 'd':
                    kputd(va_arg(argp, int), all);
                    break;

                case 'p': /* pointer, print a 0x and fall through */
                    kputs("0x", all);
                case 'x': /* lowercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_LOWER, all);
                    break;

                case 'X': /* uppercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_UPPER, all);
                    break;

                case '%':
                    kputc('%', all);
                    break;

                default:
                    /* print it raw */
                    kputc('%', all);
                    kputc(*fmt, all);
            }
        } else {
            if (*fmt == '\0') return;
            kputc(*fmt, all);
        }

        fmt++;
    }
}

void kprintf(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    kprint(fmt, argp, FALSE);
    va_end(argp);
}

void kprintf_all(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    kprint(fmt, argp, TRUE);
    va_end(argp);
}

