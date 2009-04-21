#include <ulysses/kprintf.h>
#include <ulysses/screen.h>
#include <ulysses/vt.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

/* kputc()
 *  Append a raw character. All other kput..() call this.
 */
static void kputc(const char c, flag_t all)
{
    append_char(c, TRUE, all);
}

/* kputs()
 *  Print a NULL-terminated string character-by-character.
 */
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

/* kputd()
 *  Convert a signed integer to string and and put it. 
 *  See kputdu() for unsigned.
 */
static void kputd(const int d, flag_t all)
{
    kputs(itoa(d), all);
}

/* kputdu()
 *  Convert an *un*signed integer to string and put it.
 *  See kputd() for signed.
 */
static void kputdu(const unsigned int ud, flag_t all)
{
    kputs(itoa(ud), all); /* XXX itoa() takes a signed int! */
}

/* kputx()
 *  Given the hex table to use, convert to string and put.
 *  hex_table can be either HEX_UPPER or HEX_LOWER.
 */
static void kputx(const unsigned long hex, const char hex_table[16], 
        flag_t all)
{
    unsigned long h = hex; /* since we are modifying it */
    char str[32];
    char *ptr = str + 32;

    *ptr = '\0';
    do {
        *--ptr = hex_table[(h % 0x10)];
    } while ((h /= 0x10) > 0);

    kputs(ptr, all);
}

static void do_padding(unsigned int num, char c, flag_t all)
{
    unsigned int i;
    for (i = 0; i < num; i++) {
        kputc(c, all);
    }
}

/* kprint()
 *  Formats and puts individual chars from the format string fmt. This does
 *  the actual work of kprintf() and kprintf_all().
 *  Thank you Ken Thompson for giving us this ugly mess >_<
 */
static void kprint(const char *fmt, va_list argp, flag_t all)
{
    int padding = 0;
    while (*fmt != '\0') {
        /* Format specifier, read ahead and format the arg into a string */
        if (*fmt == '%') {
            fmt++; /* skip the % since it's no longer important */
do_format: /* yuk, but the only way to get back here */
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
                    if (padding > 0) {
                        int d = va_arg(argp, int);
                        do_padding((padding - numdigits(d, 10)), '0', all);
                        kputd(d, all);
                        padding = 0;
                    } else {
                        kputd(va_arg(argp, int), all);
                    }
                    break;

                case 'p': /* pointer, print a 0x and fall through */
                    kputs("0x", all);
                case 'X': /* uppercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_UPPER, all);
                    break;

                case 'x': /* lowercase hex */
                    kputx(va_arg(argp, unsigned long), HEX_LOWER, all);
                    break;

                case '0': /* pad with 0's */
                    fmt++;
                    if (isdigit(*fmt)) {
                        char s[] = { *fmt, '\0' }; /* XXX assume len is 1 */
                        padding = strtol(s, NULL, 10);
                        fmt++;
                        goto do_format; /* process to %d now */
                    }
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
    kprint(fmt, argp, TRUE); /* print to all VTs */
    va_end(argp);
}
