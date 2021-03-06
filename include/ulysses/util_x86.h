#ifndef _ULYSSES_UTIL_X86_H
#define _ULYSSES_UTIL_X86_H

/* Shortcuts for enabling and disabling interrupts. */
#define STI __asm__ __volatile__("sti ; nop")
#define CLI __asm__ __volatile__("cli ; nop")
#define HLT while (1) __asm__ __volatile__("hlt")

/*  Convert a binary-coded decimal to an integer. */
#define BCD_INT(bcd) (((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F))

/* Byte fiddling */
#define LOW_BYTE(x) (x & 0xFF)
#define HIGH_BYTE(x) ((x >> 8) & 0xFF)

/* Perform 64-bit division through 2 32-bit registers */
#define div64(n,base) ({ \
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

/* Read the current instruction pointer and return it */
extern unsigned int read_eip(void);

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

/* Read a word (16 bits) from the given I/O port */
static inline unsigned short inw(unsigned short port)
{
    unsigned short w;
    __asm__ __volatile__("inw %w1, %w0" : "=a" (w) : "d" (port));
    return w;
}

/* Read the current value of the read time-stamp counter on the CPU
 * and return it as a 64-bit wide int for high-resolution time
 */
#define TSC_MAGIC 1662543
static inline unsigned long long rdtsc(void)
{
    unsigned int a, d;
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    return ((unsigned long long)a) | (((unsigned long long)d) << 32);
}

#endif
