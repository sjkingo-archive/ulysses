
#ifndef KHEAP_H
#define KHEAP_H

#include "oarray.h"

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000

#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

/**
   Size information for a hole/block
**/
typedef struct
{
    unsigned int magic;   // Magic number, used for error checking and identification.
    unsigned char is_hole;   // 1 if this is a hole. 0 if this is a block.
    unsigned int size;    // size of the block, including the end footer.
} header_t;

typedef struct
{
    unsigned int magic;     // Magic number, same as in header_t.
    header_t *header; // Pointer to the block header.
} footer_t;

typedef struct
{
    oarray_t index;
    unsigned int start_address; // The start of our allocated space.
    unsigned int end_address;   // The end of our allocated space. May be expanded up to max_address.
    unsigned int max_address;   // The maximum address the heap can be expanded to.
    unsigned char supervisor;     // Should extra pages requested by us be mapped as supervisor-only?
    unsigned char readonly;       // Should extra pages requested by us be mapped as read-only?
} heap_t;

/**
   Create a new heap.
**/
heap_t *create_heap(unsigned int start, unsigned int end, unsigned int max, unsigned char supervisor, unsigned char readonly);

/**
   Allocates a contiguous region of memory 'size' in size. If page_align==1, it creates that block starting
   on a page boundary.
**/
void *alloc(unsigned int size, unsigned char page_align, heap_t *heap);

/**
   Releases a block allocated with 'alloc'.
**/
void free(void *p, heap_t *heap);

/**
   Allocate a chunk of memory, sz in size. If align == 1,
   the chunk must be page-aligned. If phys != 0, the physical
   location of the allocated chunk will be stored into phys.

   This is the internal version of kmalloc. More user-friendly
   parameter representations are available in kmalloc, kmalloc_a,
   kmalloc_ap, kmalloc_p.
**/
unsigned int kmalloc_int(unsigned int sz, int align, unsigned int *phys);

/**
   Allocate a chunk of memory, sz in size. The chunk must be
   page aligned.
**/
unsigned int kmalloc_a(unsigned int sz);

/**
   Allocate a chunk of memory, sz in size. The physical address
   is returned in phys. Phys MUST be a valid pointer to unsigned int!
**/
unsigned int kmalloc_p(unsigned int sz, unsigned int *phys);

/**
   Allocate a chunk of memory, sz in size. The physical address 
   is returned in phys. It must be page-aligned.
**/
unsigned int kmalloc_ap(unsigned int sz, unsigned int *phys);

/**
   General allocation function.
**/
unsigned int kmalloc(unsigned int sz);

/**
   General deallocation function.
**/
void kfree(void *p);

#endif // KHEAP_H
