#ifndef _ULYSSES_UTIL_X86_H
#define _ULYSSES_UTIL_X86_H

/* Shortcuts for enabling and disabling interrupts. */
#define STI __asm__ __volatile__("sti ; nop")
#define CLI __asm__ __volatile__("cli ; nop")

/*  Convert a binary-coded decimal to an integer. */
#define BCD_INT(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

/* Output a byte to the given I/O port */
static inline void outb(const unsigned int port, unsigned int value)
{
    __asm__ __volatile__("outb %%al, %%dx" : : "a" (value), "d" (port));
}

/* Read a value from the given I/O port */
static inline unsigned char inb(unsigned int port)
{
    unsigned char b;
    __asm__ __volatile__("inb %w1, %b0" : "=a" (b) : "d" (port));
    return b;
}

/* Read the current value of the read time-stamp counter on the CPU
 * and return it as a 64-bit wide int for high-resolution time
 */
static inline unsigned long long rdtsc(void)
{
    unsigned int a, d;
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}

#endif
