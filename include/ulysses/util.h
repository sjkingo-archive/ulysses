#ifndef _UTIL_H
#define _UTIL_H

#define STI __asm__ __volatile__("sti ; nop")
#define CLI __asm__ __volatile__("cli ; nop")

/* Utility functions for the kernel */

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

/* stub()
 *  Print stub information for a function name given if DEBUG.
 */
void stub(char *fname);

/* print_startup()
 *  Output kernel information.
 */
void print_startup(void);

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
