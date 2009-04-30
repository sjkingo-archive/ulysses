#ifndef _ULYSSES_GDT_H
#define _ULYSSES_GDT_H

/* This is the global descriptor table that we need to load into the CPU. 
 * The GDT tells the CPU information about global memory segments, assisting
 * in protection and privilege granting for access.
 */

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
void init_gdt(void);

/* set_kernel_stack()
 *  Change to the given kernel stack in the GDT.
 */
void set_kernel_stack(unsigned int stack);

/* switch_to_ring3()
 *  Switch to privilege ring 3. This assumes we are in ring 0 -- the behaviour
 *  coming from any other ring is undefined.
 */
void switch_to_ring3(void);

/* See flush.s for assembler routines for these */
extern void gdt_flush(unsigned int);
extern void enter_pm(void);

#endif
