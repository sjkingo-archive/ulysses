#ifndef _ULYSSES_UTIL_H
#define _ULYSSES_UTIL_H

/* Utility functions for the kernel that don't really belong anywhere else. */

/* Shortcuts for enabling and disabling interrupts. */
#define STI __asm__ __volatile__("sti ; nop")
#define CLI __asm__ __volatile__("cli ; nop")

/*  Convert a binary-coded decimal to an integer. */
#define BCD_INT(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

/* print_startup()
 *  Output kernel information.
 */
void print_startup(void);

/* outb()
 *  Write a value directly to a hardware port.
 */
void outb(const unsigned int port, unsigned int value);

/* inb()
 *  Read a value directly from a hardware port.
 */
unsigned char inb(unsigned int port);

#endif
