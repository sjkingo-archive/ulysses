
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

/* kputc()
 *  Write a char directly to video memory, using the hex colour.
 */
void kputc(const char c, const char colour)
{
    unsigned char *vid = VIDMEM;
    vid[0] = c;
    vid[1] = colour;
}

/* kprintf()
 *  Accept a format string and variable arguments list, and write each char
 *  via the kputc() function.
 */
void kprintf(const char *fmt, ...)
{
    while (*fmt != NULL) {
        /* XXX actually handle format operators */
        kputc(*fmt, 0x07); /* white on black */
        fmt++;
    }
}

