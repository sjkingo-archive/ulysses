
#ifndef _x86_UTIL_H
#define _x86_UTIL_H

/* This is for all random x86-related utility functions. */

/* BCD_INT()
 *  Convert a binary-coded decimal to an integer.
 */
#define BCD_INT(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

/* outb()
 *  Write a value directly to a hardware port.
 */
void outb(const unsigned int port, unsigned int value);

/* inb()
 *  Read a value directly from a hardware port.
 */
unsigned char inb(unsigned int port);

#endif

