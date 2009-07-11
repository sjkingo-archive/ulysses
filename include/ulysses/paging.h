#ifndef _ULYSSES_PAGING_H
#define _ULYSSES_PAGING_H

/* This is the x86 paging code. It sets up a page table and tells the CPU
 * about it. This provides us with virtual memory!
 *
 * We keep two array of addresses for the page tables - one with physical,
 * one with virtual. This is required since the CPU must be told of physical
 * addresses, yet we need to deal with virtual. This incurs a 4 KB overhead
 * (one page entry) per page directory, as opposed to mapping a page table 
 * back to the directory, which wastes 256 MB of addressable space.
 */

#include <ulysses/isr.h> /* for registers_t */
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

/* init_paging()
 *  Set up paging and load the kernel's page directory into CPU.
 */
void init_paging(void);

/* page_fault()
 *  Handler for CPU interrupt 14.
 */
void page_fault(registers_t *regs);

/* get_page()
 *  Return the page entry matching the given *virtual* address from page
 *  directory dir. If the address is not assigned to a page entry and make
 *  is set, create and assign the entry.
 */
page_t *get_page(unsigned int addr, flag_t make, page_dir_t *dir);

/* alloc_frame()
 *  Allocate the frame in the given page entry.
 */
void alloc_frame(page_t *page, int is_kernel, int is_writeable);

/* free_frame()
 *  Deallocate the given page entry's frame.
 */
void free_frame(page_t *page);

/* clone_dir()
 *  Clone the given page directory and return a pointer to the new one.
 */
page_dir_t *clone_dir(page_dir_t *src);

/* move_stack()
 *  Move the kernel stack to a new location. This updates the absolute
 *  addresses of each frame in the stack. This should only be called once.
 */
void move_stack(void *new_start, unsigned int size);

/* get_kernel_dir()
 *  Returns a pointer to the current kernel page directory.
 */
page_dir_t *get_kernel_dir(void);

#endif
