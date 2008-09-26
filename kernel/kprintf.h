
#ifndef _KPRINTF_H
#define _KPRINTF_H

/* This is a very naive implementation of printf() for kernel-space. 
 * It writes directly to video memory, char by char, by calling the kputc()
 * function.
 */

/* In case someone gets sloppy... */
#define printf kprintf

#define VIDMEM ((unsigned char *)0xb8000)
#define WIDTH 80
#define HEIGHT 25

/* A screen is defined as an 80x25 location in video memory */
struct vidmem {
    unsigned char *mem;
    unsigned int next_x;
    unsigned int next_y;
};
struct vidmem screen;

void init_screen(void);
void clear_screen(void);
void kputc(const char c, const char colour);
void kprintf(const char *fmt, ...);

#endif

