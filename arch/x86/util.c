
#include "util.h"

void outb(const unsigned int port, unsigned int value)
{
    /* Intel-style, with order reversed */
    __asm__ __volatile__("outb %%al, %%dx" : : "a" (value), "d" (port));
}

unsigned char inb(unsigned int port)
{
    unsigned char b;
    __asm__ __volatile__("inb %w1, %b0" : "=a" (b) : "d" (port));
    return b;
}

