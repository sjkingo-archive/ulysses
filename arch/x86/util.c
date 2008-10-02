
#include "util.h"

void outb(const unsigned int port, unsigned int value)
{
    /* Intel-style, with order reversed */
    __asm__ __volatile__("outb %%al, %%dx" : : "a" (value), "d" (port));
}

