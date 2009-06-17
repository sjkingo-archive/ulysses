#ifndef _ULYSSES_UTIL_X86_H
#define _ULYSSES_UTIL_X86_H

/* Shortcuts for enabling and disabling interrupts. */
#define STI __asm__ __volatile__("sti ; nop")
#define CLI __asm__ __volatile__("cli ; nop")

/*  Convert a binary-coded decimal to an integer. */
#define BCD_INT(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

/* Perform 64-bit division through 2 32-bit registers */
#define do_div(n,base) ({ \
    int __res; \
    __res = ((unsigned long) n) % (unsigned) base; \
    n = ((unsigned long) n) / (unsigned) base; \
    __res; \
})

/* Trigger a page fault (for whatever reason one would have in doing so) */
#define PF __asm__ __volatile__("jmp 0x0")

/* Send a system call on software interrupt 80 */
#define INT80(x) \
({ \
    int __a; \
    __asm__ __volatile__("int $0x80" : "=a" (__a) : "0" (x)); \
    __a; \
})

/* Read the current stack pointer and return it */
#define READ_ESP() \
({ \
    int __a; \
    __asm__ __volatile__("mov %%esp, %0" : "=r" (__a)); \
    __a; \
})

/* Read the stack base pointer and return it */
#define READ_EBP() \
({ \
    int __a; \
    __asm__ __volatile__("mov %%ebp, %0" : "=r" (__a)); \
    __a; \
})

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
