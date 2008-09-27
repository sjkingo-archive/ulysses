
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

#include <stdarg.h>
#include <stdlib.h>

/* Internal function prototypes */
void kputc(const char c);
void kputs(const char *str);
void kputd(const int d);

void init_screen(void)
{
    screen.mem = VIDMEM_START;
    screen.next_x = 0;
    screen.next_y = 0;
    clear_screen();
}

void clear_screen(void)
{
    register unsigned int i;
    for (i = 0; i < (WIDTH * HEIGHT * 2); i++) {
        screen.mem[i] = ' ';
        screen.mem[++i] = COLOUR_WB; 
    }

    /* XXX reset cursor */
}

void kputc(const char c)
{
    unsigned int index;

    /* Overflow to new line if needed */
    if (screen.next_x >= WIDTH) {
        screen.next_x = 0;
        screen.next_y++;
    }

    /* Each character in video mem is 2 bytes */
    index = (screen.next_x * 2) + ((screen.next_y * 2) * WIDTH);
    screen.mem[index] = c;
    screen.mem[++index] = COLOUR_WB;
    
    screen.next_x++;
}

void kputs(const char *str)
{
    const char *ptr;
    if (str == NULL) ptr = "(null)";
    else ptr = str;

    while (*ptr != '\0') {
        kputc(*ptr);
        ptr++;
    }
}

void kputd(const int d)
{
    kputs(itoa(d));
}

void kprintf(const char *fmt, ...)
{
    va_list argp;

    /* Thank you Ken Thompson for giving us this >_< */
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

                case 'i':
                case 'd':
                    kputd(va_arg(argp, int));
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
            if (*fmt == '\n') {
                screen.next_x = 0;
                screen.next_y++;
            } else if (*fmt == '\r') {
                screen.next_x = 0;
            } else {
                kputc(*fmt);
            }
        }

        fmt++;
    }

    va_end(argp);
}

