#ifndef _TSS_H
#define _TSS_H

struct tss_entry_struct {
    unsigned int prev_tss; /* not used in software */
    unsigned int esp0; /* stack pointer to load for kernel */
    unsigned int ss0; /* stack segment to load for kernel */
    unsigned int esp1; /* not used */
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
    unsigned int fs;
    unsigned int gs;
    unsigned short ldt; /* not used */
    unsigned short iomap_base;
} __attribute__((packed));

typedef struct tss_entry_struct tss_entry_t;

#endif
