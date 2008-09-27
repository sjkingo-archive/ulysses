
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

#include <stdarg.h>

/* init_screen()
 *  Initialise video memory and clear the screen.
 */
void init_screen(void)
{
    screen.mem = VIDMEM;
    screen.next_x = 0;
    screen.next_y = 0;
    clear_screen();
}

/* clear_screen()
 *  Writes blank spaces to the current screen and reset the cursor.
 */
void clear_screen(void)
{
    register unsigned int i;
    for (i = 0; i < (WIDTH * HEIGHT * 2); i++) {
        screen.mem[i] = ' ';
        screen.mem[++i] = 0x07;
    }

    /* XXX reset cursor */
}

/* kputc()
 *  Write a char directly to video memory.
 */
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
    char *ptr;

    if (str == NULL) ptr = "(null)";
    else ptr = str;

    while (*ptr != '\0') {
        kputc(*ptr);
        ptr++;
    }
}

/* kprintf()
 *  Accept a format string and variable arguments list, and write each char
 *  via the kputc() function.
 */
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

