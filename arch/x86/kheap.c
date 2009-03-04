
#include "x86.h"
#include "kheap.h"
#include "paging.h"
#include "oarray.h"

extern unsigned int end; /* declared in linker.ld */
extern page_dir_t *kernel_directory; /* declared in paging.c */

/* placement_address
 *  Physical address to place the kernel heap at. This is usually the end
 *  of all segments, however this can be anywhere in physical memory.
 */
unsigned int placement_address = (unsigned int)&end;

/* kheap
 *  Pointer to the kernel heap.
 */
heap_t *kheap = 0;

static unsigned char header_t_less_than(void*a, void *b)
{
    return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

/* find_smallest_hole()
 *  Find the smallest hole fitting the given size in the given heap
 *  and return its index.
 */
static int find_smallest_hole(unsigned int size, unsigned char page_align, 
        heap_t *heap)
{
    unsigned int i = 0;
    while (i < heap->index.size) {
        header_t *header = (header_t *)lookup_oarray(i, &heap->index);

        /* Page align the starting point of the header */
        if (page_align > 0) {
            unsigned int loc, offset;
            int hole_size;

            loc = (unsigned int)header;
            offset = 0;
            if (((loc + sizeof(header_t)) & 0xFFFFF000) != 0) {
                offset = 0x1000 - (loc + sizeof(header_t)) % 0x1000;
            }
            
            /* Break if we fit now */
            hole_size = (int)header->size - offset;
            if (hole_size >= (int)size) {
                break;
            }
        } else if (header->size >= size) {
            break;
        }

        i++;
    }

    if (i == heap->index.size) {
        return -1; /* nothing found */
    } else {
        return i;
    }
}

/* expand()
 *  Expand the given heap to the given size.
 */
static void expand(unsigned int new_size, heap_t *heap)
{
    unsigned int old_size, i;

    /* Find the nearest page boundary following this */
    if ((new_size & 0xFFFFF000) != 0) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }
    old_size = heap->end_address-heap->start_address;

    /* Allocate a new frame */
    i = old_size;
    while (i < new_size) {
        alloc_frame(get_page(heap->start_address + i, 1, kernel_directory),
                     (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
        i += 0x1000; /* page size */
    }
    heap->end_address = heap->start_address + new_size;
}

/* contact()
 *  Shrink the given heap to the given size. Note that new_size is the
 *  *new* size, not the size to shrink!
 */
static unsigned int contract(unsigned int new_size, heap_t *heap)
{
    unsigned int old_size, i;

    /* Find the nearest page boundary following this */
    if (new_size & 0x1000) {
        new_size &= 0x1000;
        new_size += 0x1000;
    }
    if (new_size < HEAP_MIN_SIZE) new_size = HEAP_MIN_SIZE;

    old_size = heap->end_address - heap->start_address;
    i = old_size - 0x1000;
    while (new_size < i) {
        free_frame(get_page(heap->start_address + i, 0, kernel_directory));
        i -= 0x1000;
    }

    heap->end_address = heap->start_address + new_size;
    return new_size;
}

/* alloc()
 *  Allocates a contiguous region of memory with the given size, in the 
 *  given heap. If page_align is 1, align the block on a page boundary.
 *  Returns a pointer to the block.
 */
static void *alloc(unsigned int size, unsigned char page_align, heap_t *heap)
{
    int i;
    unsigned int new_size;

    new_size = size + sizeof(header_t) + sizeof(footer_t);
    i = find_smallest_hole(new_size, page_align, heap);

    /* If there was no hole big enough for the size we need, expand the heap */
    if (i == -1) {
        int index;
        unsigned int old_length, old_end_address, new_length, j, val;

        old_length = heap->end_address - heap->start_address;
        old_end_address = heap->end_address;

        /* Expand the heap */
        expand(old_length + new_size, heap);
        new_length = heap->end_address - heap->start_address;

        /* Find the endmost header (by location) */
        j = 0;
        index = -1;
        val = 0x0;
        while (j < heap->index.size) {
            unsigned int tmp = (unsigned int)lookup_oarray(i, &heap->index);
            if (tmp > val) {
                val = tmp;
                index = i;
            }
            j++;
        }

        /* If there were no headers found (by location), add one */
        if (index == -1) {
            header_t *header = (header_t *)old_end_address;
            header->magic = HEAP_MAGIC;
            header->size = new_length - old_length;
            header->is_hole = 1;
            footer_t *footer = (footer_t *)(old_end_address + header->size - 
                    sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
            insert_oarray((void *)header, &heap->index);
        } else {
            header_t *header = lookup_oarray(index, &heap->index);
            header->size += new_length - old_length;
            footer_t *footer = (footer_t *)((unsigned int)header + 
                    header->size - sizeof(footer_t));
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }

        /* Heap has been expanded, recurse */
        return alloc(size, page_align, heap);
    }

    /* Some more variables for us - this won't work with -ansi! */
    unsigned int orig_hole_pos, orig_hole_size;

    header_t *orig_hole_header = (header_t *)lookup_oarray(i, &heap->index);
    orig_hole_pos = (unsigned int)orig_hole_header;
    orig_hole_size = orig_hole_header->size;

    /* Work out if there is less overhead in splitting the hole into
     * two parts, rather than adding a new hole. If so, increase the 
     * requested size of the hole.
     */
    if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
        size += orig_hole_size-new_size;
        new_size = orig_hole_size;
    }

    /* Page align the new hole if needed */
    if (page_align && orig_hole_pos & 0xFFFFF000) {
        unsigned int new_location = orig_hole_pos + 0x1000 - 
                (orig_hole_pos & 0xFFF) - sizeof(header_t);
        header_t *hole_header = (header_t *)orig_hole_pos;
        hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = 1;
        footer_t *hole_footer = (footer_t *)((unsigned int)new_location - sizeof(footer_t));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;
        orig_hole_pos = new_location;
        orig_hole_size = orig_hole_size - hole_header->size;
    } else {
        /* We don't need the hole any more, remove it */
        remove_oarray(i, &heap->index);
    }

    /* Overwrite the original header and footer */
    header_t *block_header = (header_t *)orig_hole_pos;
    block_header->magic = HEAP_MAGIC;
    block_header->is_hole = 0;
    block_header->size = new_size;
    footer_t *block_footer = (footer_t *)(orig_hole_pos + sizeof(header_t) + size);
    block_footer->magic = HEAP_MAGIC;
    block_footer->header = block_header;

    /* Write a new hole after the allocated block */
    if (orig_hole_size - new_size > 0) {
        header_t *hole_header = (header_t *)(orig_hole_pos + 
                sizeof(header_t) + size + sizeof(footer_t));
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = 1;
        hole_header->size = orig_hole_size - new_size;
        footer_t *hole_footer = (footer_t *)((unsigned int)hole_header + 
                orig_hole_size - new_size - sizeof(footer_t));

        if ((unsigned int)hole_footer < heap->end_address) {
            hole_footer->magic = HEAP_MAGIC;
            hole_footer->header = hole_header;
        }
        insert_oarray((void*)hole_header, &heap->index);
    }
    
    return (void *)((unsigned int)block_header + sizeof(header_t));
}

/* free()
 *  Deallocates a block allocated by alloc() in the given heap.
 */
static void free(void *p, heap_t *heap)
{
    header_t *header, *test_header;
    footer_t *footer, *test_footer;
    char do_add;

    if (p == 0) {
        return;
    }

    header = (header_t*)((unsigned int)p - sizeof(header_t));
    footer = (footer_t*)((unsigned int)header + header->size - 
            sizeof(footer_t));

    header->is_hole = 1;
    do_add = 1;

    /* Test if the the block directly left is a footer */
    test_footer = (footer_t *)((unsigned int)header - sizeof(footer_t));
    if (test_footer->magic == HEAP_MAGIC && 
            test_footer->header->is_hole == 1) {
        unsigned int cache_size = header->size;
        header = test_footer->header;
        footer->header = header;
        header->size += cache_size;
        do_add = 0; /* don't duplicate in index */
    }

    /* Test if the block directly right is a header */
    test_header = (header_t *)((unsigned int)footer + sizeof(footer_t));
    if (test_header->magic == HEAP_MAGIC &&
            test_header->is_hole) {
        unsigned int i;

        header->size += test_header->size;
        test_footer = (footer_t *)((unsigned int)test_header +
                test_header->size - sizeof(footer_t));
        footer = test_footer;

        /* Remove from index */
        i = 0;
        while ((i < heap->index.size) && (lookup_oarray(i, &heap->index) != 
                    (void *)test_header)) {
            i++;
        }
        remove_oarray(i, &heap->index);
    }

    /* If this is the last block in the heap, contact the heap */
    if ((unsigned int)footer + sizeof(footer_t) == heap->end_address) {
        unsigned int old_length, new_length;
        
        old_length = heap->end_address - heap->start_address;
        new_length = contract((unsigned int)header - heap->start_address, 
                heap);
        
        if (header->size - (old_length - new_length) > 0) {
            /* Resize */
            header->size -= old_length - new_length;
            footer = (footer_t *)((unsigned int)header + header->size - 
                    sizeof(footer_t));
            footer->magic = HEAP_MAGIC;
            footer->header = header;
        } else {
            /* Remove from index */
            unsigned int i = 0;
            while ((i < heap->index.size) && (lookup_oarray(i, 
                            &heap->index) != (void *)test_header)) {
                i++;
            }
            
            /* Nothing found, remove the array */
            if (i < heap->index.size) {
                remove_oarray(i, &heap->index);
            }
        }
    }

    if (do_add == 1) {
        /* Add to index */
        insert_oarray((void *)header, &heap->index);
    }
}

/* kmalloc_int()
 *  Internal implementation of kmalloc(): allocate a block with the given
 *  size in the kernel heap.
 */
static void *kmalloc_int(unsigned int sz, int align, unsigned int *phys)
{
    if (kheap != 0) {
        void *addr = alloc(sz, (unsigned char)align, kheap);
        if (phys != 0) {
            page_t *page = get_page((unsigned int)addr, 0, kernel_directory);
            *phys = page->frame * 0x1000 + ((unsigned int)addr & 0xFFF);
        }
        return addr;
    } else {
        unsigned int tmp;

        /* Align the placement address */
        if (align == 1 && (placement_address & 0xFFFFF000)) {
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }
        if (phys) *phys = placement_address;
        
        tmp = placement_address;
        placement_address += sz;
        return (void *)tmp;
    }
}

heap_t *create_heap(unsigned int start, unsigned int end_addr, 
        unsigned int max, unsigned char supervisor, unsigned char readonly)
{
    heap_t *heap;
    
    /* Initialize the heap */
    heap = (heap_t *)kmalloc(sizeof(heap_t));
    heap->index = place_oarray((void *)start, HEAP_INDEX_SIZE, &header_t_less_than);
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    /* The start address needs to be page-aligned */
    if ((start & 0xFFFFF000) != 0) {
        start &= 0xFFFFF000;
        start += 0x1000;
    }

    heap->start_address = start;
    heap->end_address = end_addr;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    /* The heap starts with one large hole in the index */
    header_t *hole = (header_t *)start;
    hole->size = end_addr-start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    insert_oarray((void *)hole, &heap->index);     

    return heap;
}

void *kmalloc(unsigned int size)
{
    return kmalloc_int(size, 0, 0);
}

void kfree(void *p)
{
    free(p, kheap);
}

void *kmalloc_a(unsigned int size)
{
    return kmalloc_int(size, 1, 0);
}

void *kmalloc_ap(unsigned int size, unsigned int *phys)
{
    return kmalloc_int(size, 1, phys);
} 

