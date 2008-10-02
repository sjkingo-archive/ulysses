
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

#include <stdarg.h>
#include <stdlib.h>

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
    move_cursor(0, 0);
}

void move_cursor(const unsigned int x, const unsigned int y)
{
    unsigned int index = x + (WIDTH * y);

    /* High byte */
    outb(0x3d4, 0xe); /* CRTC register */
    outb(0x3d5, (index >> 8)); /* data */

    /* Low byte */
    outb(0x3d4, 0xf); /* CRTC register */
    outb(0x3d5, (index & 0xFF)); /* data */
}

/* scroll_screen()
 *  Move every character in video memory up one line. This is a very slow 
 *  and inefficient process, so we request GCC to place all variables on
 *  CPU registers instead of stack.
 */
static void scroll_screen(void)
{
    /* Each char in memory is 2 bytes wide, so we need to deal with both when
     * moving.
     */
    register unsigned short h = HEIGHT * 2;
    register unsigned short w = WIDTH * 2;
    register unsigned short last_line = HEIGHT - 1;

    register unsigned short i;
    for (i = 0; i < (h * w); i++) {
        screen.mem[i - w] = screen.mem[i]; /* move one line backwards */
    }

    /* NOTE: do not update x! */
    screen.next_y = last_line; /* write to the last line */
    move_cursor(screen.next_x, last_line);
}

static void kputc(const char c)
{
    unsigned int index;

    /* Overflow to new line if needed */
    if (screen.next_x >= WIDTH) {
        screen.next_x = 0;
        screen.next_y++;
    }

    /* Overflow the screen if needed */
    if (screen.next_y >= HEIGHT) scroll_screen();

    /* Each character in video mem is 2 bytes */
    index = (screen.next_x * 2) + ((screen.next_y * 2) * WIDTH);
    screen.mem[index] = c;
    screen.mem[++index] = COLOUR_WB;

    /* Update the screen cursor (not hardware). This will place the cursor
     * underneath the current char.
     */
    move_cursor(screen.next_x, screen.next_y);
    
    screen.next_x++;
}

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
            switch (*fmt) {

            case '\0':
                return;

            case '\b':
                if (screen.next_x) screen.next_x--;
                break;

            case '\n':
                screen.next_y++;
                /* fall through to \r */
            case '\r':
                screen.next_x = 0;
                break;

            case '\t':
                /* XXX still doesn't work 100% */
                screen.next_x = (screen.next_x + 8) & ~(8 - 1);
                break;

            default:
                kputc(*fmt);

            }
        }

        fmt++;
    }

    va_end(argp);
}

