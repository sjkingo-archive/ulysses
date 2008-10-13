
#ifndef _PAGING_H
#define _PAGING_H

/* This is the x86 paging code. It sets up a page table and tells the CPU
 * about it. This provides us with virtual memory!
 */

#include "isr/isr.h" /* for registers_t */
#include <types.h> /* for flag_t */

#define INDEX_FROM_BIT(a) (a/32)
#define OFFSET_FROM_BIT(a) (a%32)

typedef struct page {
    unsigned int present : 1; /* page present in memeory */
    unsigned int rw : 1;
    unsigned int user : 1;
    unsigned int accessed : 1; /* accessed since last refresh */
    unsigned int dirty : 1; /* written to since last refresh */
    unsigned int unused : 7; /* don't ask */
    unsigned int frame : 20; /* address, bitwise shift right 12 bits */
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t *tables[1024]; /* pagetables */
    unsigned int tables_phys[1024]; /* array of points to pagetables */
    unsigned int phys_addr; /* address of tables_phys */
} page_dir_t;

unsigned int *frames; /* pointer to first frame */
unsigned int nframes; /* number of frames */
unsigned int placement_address; /* end address */

page_dir_t *kernel_directory; /* kernel's page directory */
page_dir_t *current_directory; /* current page directory */

flag_t init_paging(void);

void page_fault(registers_t regs);

#endif

