
#ifndef _x86_PAGING_H
#define _x86_PAGING_H

/* This is the x86 paging code. It sets up a page table and tells the CPU
 * about it. This provides us with virtual memory!
 *
 * We keep two array of addresses for the page tables - one with physical,
 * one with virtual. This is required since the CPU must be told of physical
 * addresses, yet we need to deal with virtual. This incurs a 4 KB overhead
 * (one page entry) per page directory, as opposed to mapping a page table 
 * back to the directory, which wastes 256 MB of addressable space.
 */

#include "isr/isr.h" /* for registers_t */
#include <sys/types.h> /* for flag_t */

#define INDEX_FROM_BIT(a) (a/32)
#define OFFSET_FROM_BIT(a) (a%32)

typedef struct page {
    unsigned int present : 1; /* page present in memeory */
    unsigned int rw : 1; /* writable */
    unsigned int user : 1; /* user/kernel space */
    unsigned int accessed : 1; /* accessed since last refresh */
    unsigned int dirty : 1; /* written to since last refresh */
    unsigned int unused : 7; /* don't ask */
    unsigned int frame : 20; /* address, bitwise shift right 12 bits */
} page_t; /* page entry */

typedef struct page_table {
    page_t pages[1024]; /* 1024 page entries per table */
} page_table_t; /* page table */

typedef struct page_directory {
    page_table_t *tables[1024]; /* page tables */
    unsigned int tables_phys[1024]; /* array of pointers to the tables */
    unsigned int phys_addr; /* *physical* address of tables_phys */
} page_dir_t;

unsigned int *frames; /* pointer to first frame */
unsigned int nframes; /* number of frames */
unsigned int placement_address; /* end address of physical memory */

page_dir_t *kernel_directory; /* kernel's page directory */
page_dir_t *current_directory; /* current page directory */

/* init_paging()
 *  Set up paging and load the kernel's page directory into CPU.
 */
flag_t init_paging(void);

/* page_fault()
 *  Handler for CPU interrupt 14.
 */
void page_fault(registers_t regs);

#endif

