
/* kernel.h includes kprintf.h for us */
#include "kernel.h"

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
 *  Write a char directly to video memory, using the hex colour.
 */
void kputc(const char c, const char colour)
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
    screen.mem[++index] = colour;
    
    screen.next_x++;
}

/* kprintf()
 *  Accept a format string and variable arguments list, and write each char
 *  via the kputc() function.
 */
void kprintf(const char *fmt, ...)
{
    while (*fmt != '\0') {
        /* XXX actually handle format operators */
        kputc(*fmt, 0x07); /* white on black */
        fmt++;
    }
}

