
#ifndef _KPRINTF_H
#define _KPRINTF_H

/* This is a very naive implementation of printf() for kernel-space. 
 * It writes directly to video memory, char by char, by calling the kputc()
 * function.
 */

/* In case someone gets sloppy... */
#define printf kprintf

#define VIDMEM ((unsigned char *)0xb8000)

void kputc(const char c, const char colour);

void kprintf(const char *fmt, ...);

#endif

