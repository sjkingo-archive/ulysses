#ifndef _ULYSSES_SCREEN_H
#define _ULYSSES_SCREEN_H

/* This is the abstraction for a screen in video memory. We declare this 
 * screen to be a "square" in video memory.
 */

#define VIDMEM_START ((unsigned char *)0xb8000)
#define WIDTH 80
#define HEIGHT 25

enum screen_colours {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LGRAY,
    DGRAY,
    LBLUE,
    LGREEN,
    LCYAN,
    LRED,
    LMAGENTA,
    LYELLOW,
    WHITE,
    DEFAULT,
};

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
 *  Put a single character to screen with the given colour code.
 */
void put_char(const char c);

/* flush_screen()
 *  Clear the screen and write data to it.
 */
void flush_screen(const char *data);

/* change_colour()
 *  Change the current screen colour to the (bg,fg) pair given.
 *  This will affect any further characters (re)wrtten to the screen.
 */
void change_colour(const char bg, const char fg);

#endif
