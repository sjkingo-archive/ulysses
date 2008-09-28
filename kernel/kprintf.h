
#ifndef _KPRINTF_H
#define _KPRINTF_H

/* Here be the kernel printing stuff! We define a "screen" as a location
 * in video memory, and provide kprintf() to write to it. There is no buffer
 * for this - instead we write directly to video memory, char-by-char.
 */

/* In case someone gets sloppy... */
#define printf kprintf

#define VIDMEM_START ((unsigned char *)0xb8000)
#define WIDTH 80
#define HEIGHT 25

/* Colours */
#define COLOUR_WB 0x07 /* white on black */

#define TAB "        " /* 8 spaces */

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

/* kprintf()
 *  printf() for the kernel. Takes a format string and variable arguments
 *  list, formats the string using given args and specifiers, and appends
 *  the formatted string to the screen.
 */
void kprintf(const char *fmt, ...);

#endif

