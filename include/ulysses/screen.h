#ifndef _ULYSSES_SCREEN_H
#define _ULYSSES_SCREEN_H

/* This is the abstraction for a screen in video memory. We declare this 
 * screen to be a "square" in video memory.
 */

#define VIDEO_MEM_START 0xB8000

/* Dimensions of the screen */
#define SCREEN_COLS 80
#define SCREEN_ROWS 25

/* Some useful macros */
#define PAIR_TO_ATTRIB(bg, fg) ((bg << 4) | (fg & 0x0F))
#define COORD_TO_OFFSET(x, y) ((y * SCREEN_COLS) + x)

struct video_cell {
    unsigned char c;
    unsigned char attrib;
};

struct video_framebuffer {
    struct video_cell *mem;
    unsigned short next_coord_x;
    unsigned short next_coord_y;
};

/* Colour values that make up attributes */
enum screen_colours {
    COLOUR_BLACK=0x00,
    COLOUR_BLUE=0x01,
    COLOUR_GREEN=0x02,
    COLOUR_CYAN=0x03,
    COLOUR_RED=0x04,
    COLOUR_MAGENTA=0x05,
    COLOUR_BROWN=0x06,
    COLOUR_LGRAY=0x07,
    COLOUR_DGRAY=0x08,
    COLOUR_LBLUE=0x09,
    COLOUR_LGREEN=0x10,
    COLOUR_LCYAN=0x11,
    COLOUR_LRED=0x12,
    COLOUR_LMAGENTA=0x13,
    COLOUR_LBROWN=0x14,
    COLOUR_WHITE=0x15,
};

/* init_screen()
 *  Initialises the first 80x25 screen in memory and clears it, ready to be
 *  written to.
 */
void init_screen(void);

/* put_char()
 *  Put a single character to screen with the given colour code.
 */
void put_char(unsigned char c);

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
