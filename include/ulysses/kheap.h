#ifndef _ULYSSES_KHEAP_H
#define _ULYSSES_KHEAP_H

/* The kernel heap is a large pool of memory that can be allocated to kernel
 * data structures and modules. We start with a small heap and expand as 
 * allocations are requested.
 *
 * Since this is a simple implementation, we make no effort to relocate any
 * data in memory to merge holes in the heap -- this could very well mean the 
 * heap is filled if a lot of allocations are performed. If this happens, the
 * kernel will panic.
 *
 * The structure of this module allows multiple heaps to be created, although
 * we initially only create one for the kernel.
 */

#include <ulysses/oarray.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000

/* Block header */
typedef struct {
    unsigned int magic; /* set to KHEAP_MAGIC */
    unsigned char is_hole; /* 1: hole; 0: allocated block */
    unsigned int size; /* size, including footer */
} header_t;

/* Block footer */
typedef struct {
    unsigned int magic; /* set to KHEAP_MAGIC */
    header_t *header;
} footer_t;

/* The heap */
typedef struct {
    oarray_t index;
    unsigned int start_address;
    unsigned int end_address;
    unsigned int max_address;
    unsigned char supervisor;
    unsigned char readonly;
} heap_t;

/* create_heap()
 *  Create a new heap with the given flags and return a pointer to it.
 */
heap_t *create_heap(unsigned int start, unsigned int end, unsigned int max, 
        unsigned char supervisor, unsigned char readonly);

/* kmalloc()
 *  Allocate a chunk of memory of the given size and return the pointer
 *  to it. Returns NULL if memory could not be allocated.
 */
void *kmalloc(unsigned int size);

/* kfree()
 *  Deallocate the given pointer that was previously return from kmalloc().
 */
void kfree(void *p);

/* kmalloc_a()
 *  Allocate a chunk of page-aligned memory of the given size. Returns
 *  the same as kmalloc().
 */
void *kmalloc_a(unsigned int size);

/* kmalloc_ap()
 *  Allocate a chunk of page-aligned physical memory of the given size.
 *  This is only useful before virtual memory is set up. Returns the
 *  same as kmalloc().
 */
void *kmalloc_ap(unsigned int size, unsigned int *phys);

#endif
