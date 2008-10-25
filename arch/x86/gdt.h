
#ifndef _x86_GDT_H
#define _x86_GDT_H

/* This is the global descriptor table that x86 kindly gives us segmentation
 * through.
 */

#include <sys/types.h>

struct gdt_entry_struct {
    unsigned short limit_low;
    unsigned short base_low; /* lowest 16 bits of the base */
    unsigned char base_middle; /* next 16 bits of the base */
    unsigned char access; /* access ring */
    unsigned char granularity;
    unsigned char base_high; /* highest 8 bits of the base */
} __attribute__((packed)); /* pack to 1 byte */
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct {
    unsigned short limit; /* upper 16 bits of all limits */
    unsigned int base; /* address of the first gdt_entry_t */
} __attribute__((packed)); /* pack to 1 byte */
typedef struct gdt_ptr_struct gdt_ptr_t;

gdt_ptr_t gdt_ptr;
gdt_entry_t gdt_entries[5];

/* init_gdt()
 *  Initialise the global descriptor table with a lot of bitwise and
 *  assembler operations.
 */
flag_t init_gdt(void);

/* See flush.s for assembler routine for this. */
/* XXX move this to inline assembler */
extern void gdt_flush(unsigned int);

#endif

