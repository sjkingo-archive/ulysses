#ifndef _ULYSSES_SCREEN_H
#define _ULYSSES_SCREEN_H

/* This is the abstraction for a screen in video memory. We declare this 
 * screen to be a "square" in video memory.
 */

#define VIDMEM_START ((unsigned char *)0xb8000)
#define WIDTH 80
#define HEIGHT 25

/* Colours */
#define COLOUR_WB 0x07 /* white on black */

#define TAB "        " /* 8 spaces */

/* The hexidecimal alphabet */
#define HEX_LOWER "0123456789abcdef"
#define HEX_UPPER "0123456789ABCDEF"

/* A screen is defined as an 80x25 location in video memory */
struct vidmem {
    unsigned char *mem;
    unsigned int next_x;
    unsigned int next_y;
};
struct vidmem screen;

/* init_screen()
 *  Initialises the first 80x25 screen in memory and clears it, ready to be
 *  written to.
 */
void init_screen(void);

/* clear_screen()
 *  Clear the screen by writing blank spaces to it. Resets the cursor to 0x0.
 */
void clear_screen(void);

/* move_cursor()
 *  Move the screen cursor to the specified coordinates.
 */
void move_cursor(const unsigned int x, const unsigned int y);

/* put_char()
 *  Put a single character to screen.
 */
void put_char(const char c);

/* flush_screen()
 *  Clear the screen and write data to it.
 */
void flush_screen(const char *data);

#endif
