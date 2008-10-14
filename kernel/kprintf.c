
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

#include <stdarg.h>
#include <stdlib.h>

static void kputs(const char *str)
{
    const char *ptr; /* since we may want to change what str is pointing to */
    if (str == NULL) ptr = "(null)";
    else ptr = str;

    while (*ptr != '\0') {
        kputc(*ptr);
        ptr++;
    }
}

static void kputd(const int d)
{
    kputs(itoa(d));
}

static void kputdu(const unsigned int ud)
{
    kputs(itoa(ud)); /* XXX itoa() takes a signed int! */
}

static void kputx(const unsigned long hex, const char hex_table[16])
{
    unsigned long h = hex; /* since we are modifying it */
    char *str;

    do {
        *--str = hex_table[(h % 0x10)];
    } while ((h /= 0x10) > 0);

    kputs(str);
}

void kputc(const char c)
{
    append_char(c, TRUE);
}

void kprintf(const char *fmt, ...)
{
    /* Thank you Ken Thompson for giving us this >_< */
    va_list argp;
    va_start(argp, fmt);

    while (*fmt != '\0') {
        /* Format specifier, read ahead and format the arg into a string */
        if (*fmt == '%') {
            fmt++; /* skip the % since it's no longer important */
            switch (*fmt) {
                case 's':
                    kputs(va_arg(argp, char *));
                    break;

                case 'c':
                    kputc(va_arg(argp, char));
                    break;

                case 'u': /* unsigned int */
                    kputdu(va_arg(argp, unsigned int));
                    break;

                case 'i': /* signed int */
                case 'd':
                    kputd(va_arg(argp, int));
                    break;

                case 'p': /* pointer, print a 0x and fall through */
                    kputs("0x");
                case 'x': /* lowercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_LOWER);
                    break;

                case 'X': /* uppercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_UPPER);
                    break;

                case '%':
                    kputc('%');
                    break;

                default:
                    /* print it raw */
                    kputc('%');
                    kputc(*fmt);
            }
        } else {
            if (*fmt == '\0') return;
            kputc(*fmt);
        }

        fmt++;
    }

    va_end(argp);
}

