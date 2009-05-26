#ifndef _ULYSSES_HEAP_H
#define _ULYSSES_HEAP_H

#include <sys/types.h>

struct block {
    unsigned int magic;
    unsigned short free;
    size_t size;
    struct block *next;
};

/* new_heap()
 *  Creates a new heap of the given size at the memory address given by 
 *  `start`. Return the address used.
 */
void *new_heap(void *start, size_t size);

/* dump_heap()
 *  Dump the given heap to screen.
 */
void dump_heap(void *start);

/* heap_check()
 *  Perform a consistency check on the heap given by `start`.
 */
void heap_check(void *start);

/* alloc()
 *  Wrapper for kmalloc().
 */
void *alloc(size_t size);

#endif
