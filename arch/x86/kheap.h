
#ifndef KHEAP_H
#define KHEAP_H

#include "oarray.h"

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000

typedef struct {
    unsigned int magic; /* set to KHEAP_MAGIC */
    unsigned char is_hole; /* 1: hole; 0: allocated block */
    unsigned int size; /* size, including footer */
} header_t;

typedef struct {
    unsigned int magic; /* set to KHEAP_MAGIC */
    header_t *header;
} footer_t;

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

