
#ifndef _x86_UTIL_H
#define _x86_UTIL_H

/* This is for all random x86-related utility functions. */

/* outb()
 *  Write a value directly to a hardware port.
 */
void outb(const unsigned int port, unsigned int value);

unsigned char inb(unsigned int port);

#endif

