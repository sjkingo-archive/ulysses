/* arch/x86/screen.c - low-level screen handling
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/screen.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

/* Turn (bg,fg) pair into colour attribute */
#define COLOUR(b, f) ((b << 4) | (f & 0x0F))

/* the current colour attribute to use when writing to screen */
static char current_colour;

void init_screen(void)
{
    screen.mem = VIDMEM_START;
    change_colour(BLACK, WHITE);
    clear_screen();
}

void clear_screen(void)
{
    TRACE_ONCE;
    register unsigned int i;
    for (i = 0; i < (WIDTH * HEIGHT * 2); i++) {
        screen.mem[i] = ' ';
        screen.mem[++i] = current_colour;
    }
    move_cursor(0, 0);
    screen.next_x = 0;
    screen.next_y = 0;
}

void move_cursor(const unsigned int x, const unsigned int y)
{
    TRACE_ONCE;
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
    TRACE_ONCE;
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

void put_char(const char c)
{
    TRACE_ONCE;
    unsigned int index;

    /* Handle any cursor manipulations given by format specifiers */
    switch (c) {
        case '\b':
            if (screen.next_x) screen.next_x--;
            return;

        case '\n':
            screen.next_y++;
            /* fall through to \r */
        case '\r':
            screen.next_x = 0;
            return;

        case '\t':
            /* XXX still doesn't work */
            screen.next_x = (screen.next_x + 8) & ~(8 - 1);
            return;
    }

    /* Overflow to new line if needed */
    if (screen.next_x >= WIDTH) {
        screen.next_x = 0;
        screen.next_y++;
    }

    /* Overflow the screen if needed */
    if (screen.next_y >= HEIGHT) scroll_screen();

    /* Each character in video mem is 2 bytes: we call this a cell */
    index = (screen.next_x * 2) + ((screen.next_y * 2) * WIDTH);
    screen.mem[index] = c;
    screen.mem[++index] = current_colour;
    
    /* Incremement the x position and move the cursor to the next cell */
    move_cursor(++screen.next_x, screen.next_y);
}

void flush_screen(const char *data)
{
    TRACE_ONCE;
    clear_screen();
    while (*data) {
        put_char(*data);
        data++;
    }
}

void change_colour(const char bg, const char fg)
{
    TRACE_ONCE;
    current_colour = COLOUR(bg, fg);
}
