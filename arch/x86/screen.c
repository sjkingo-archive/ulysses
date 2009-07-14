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

/* the screen */
static struct video_framebuffer screen;

/* the current colour attribute to use when writing to screen */
static unsigned char current_bg;
static unsigned char current_fg;

/* move_cursor()
 *  Move the screen cursor to the specified coordinates.
 */
static inline void move_cursor(unsigned short x, unsigned short y)
{
    TRACE_ONCE;
    unsigned short index = COORD_TO_OFFSET(x, y);

    /* High byte */
    outb(0x3D4, 0xE); /* CRTC register */
    outb(0x3D5, (index >> 8)); /* data */

    /* Low byte */
    outb(0x3D4, 0xF); /* CRTC register */
    outb(0x3D5, (index & 0xFF)); /* data */
}

/* scroll_screen()
 *  Move every character in video memory up one line.
 *  Note that this is a very slow and inefficient process!
 */
static void scroll_screen(void)
{
    TRACE_ONCE;

    unsigned short i;
    for (i = 0; i < SCREEN_COLS * SCREEN_ROWS * 2; i++) {
        struct video_cell cell;
        cell = (struct video_cell)screen.mem[i];
        screen.mem[i-SCREEN_COLS] = cell; /* move backwards */
    }

    /* NOTE: do not update x! */
    screen.next_coord_y = SCREEN_ROWS - 1; /* write to the last line */
    move_cursor(screen.next_coord_x, screen.next_coord_y);
}

/* clear_screen()
 *  Clear the screen by writing blank spaces to it. Resets the cursor to 0x0.
 */
static void clear_screen(void)
{
    TRACE_ONCE;
    
    unsigned short i;
    for (i = 0; i < SCREEN_COLS * SCREEN_ROWS * 2; i++) {
        struct video_cell cell;
        cell.c = ' ';
        cell.attrib = PAIR_TO_ATTRIB(current_bg, current_fg);
        screen.mem[i] = cell;
    }

    move_cursor(0, 0);
    screen.next_coord_x = 0;
    screen.next_coord_y = 0;
}

void init_screen(void)
{
    TRACE_ONCE;

    screen.mem = (struct video_cell *)VIDEO_MEM_START;
    change_colour(COLOUR_BLACK, COLOUR_LGRAY);
    clear_screen();
}

void put_char(unsigned char c)
{
    TRACE_ONCE;

    struct video_cell this_cell;
    unsigned short offset;

    /* Handle any cursor manipulations given by format specifiers */
    switch (c) {
        case '\b':
            if (screen.next_coord_x > 0) {
                screen.next_coord_x--;
            }
            return;

        case '\n':
            screen.next_coord_y++;
            /* fall through to \r */
        case '\r':
            screen.next_coord_x = 0;
            move_cursor(screen.next_coord_y, screen.next_coord_y);
            return;

        case '\t':
            /* XXX still doesn't work */
            screen.next_coord_x = (screen.next_coord_x + 8) & ~(8 - 1);
            return;
    }

    /* Overflow to new line if needed */
    if (screen.next_coord_x >= SCREEN_COLS) {
        screen.next_coord_x = 0;
        screen.next_coord_y++;
    }

    /* Overflow the screen if needed */
    if (screen.next_coord_y >= SCREEN_ROWS) {
        scroll_screen();
    }

    /* Populate this cell */
    this_cell.c = c;
    this_cell.attrib = PAIR_TO_ATTRIB(current_bg, current_fg);
    offset = COORD_TO_OFFSET(screen.next_coord_x, screen.next_coord_y);
    screen.mem[offset] = this_cell;
    
    /* Incremement the x position and move the cursor to the next cell */
    move_cursor(++screen.next_coord_x, screen.next_coord_y);
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

    current_bg = bg;
    current_fg = fg;
}
