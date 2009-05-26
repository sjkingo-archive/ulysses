/* heap.c - a first-fit memory manager
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/heap.h>
#include <ulysses/kprintf.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_DEBUG 0
#define HEAP_SIZE 1024
#define HEAP_MAGIC 0xCABEE

#define DATA(ptr) (ptr + sizeof(struct block))

static void *heap_start;

static struct block *new_block(void *dest, size_t size)
{
    struct block b;
    b.magic = HEAP_MAGIC;
    b.free = 1;
    b.size = size;
    b.next = NULL;
    memcpy(dest, &b, sizeof(struct block));
#if HEAP_DEBUG
    kprintf("new_block(): at %p; %d bytes\n", dest, (int)size);
#endif
    return (struct block *)dest;
}

static void *split_block(struct block *b)
{
    void *new_pos;
    struct block *old_next, *new;
    int new_size; /* needs to be signed so we can check for errors */
    
    /* Don't trample on used blocks! */
    assert(b->free == 1);
    
    old_next = b->next;
    
    /* Work out the new size. Since we're splitting the *data* of the given
     * block, we must take into account the extra block header we need to
     * add. This can be shown as such:
     *
     * Block before splitting:
     *
     * ||  10    |       30       ||  = 40 bytes
     * ------------------------------->
     * || header |      data      || ->
     * ------------------------------->
     *                    ^ new insertion point
     *
     * Block after splitting:
     *
     * ||  10    |  10  ||  10    |  10  ||   = 40 bytes
     * -------------------------------------->
     * || header | data || header | data || ->
     * -------------------------------------->
     *                  ^ new block
     *
     * If splitting the block would result in an invalid data size, we cannot
     * continue, so just return NULL.
     */
    new_size = (b->size - sizeof(struct block)) / 2;
    if (new_size <= 0) {
#if HEAP_DEBUG
        kprintf("split_block(): cannot split block %p (%d bytes) since "
                "it will be %d bytes\n", b, (int)b->size, new_size);
#endif
        return NULL;
    }
    b->size = new_size;
#if HEAP_DEBUG
    kprintf("split_block(): block at %p now has size %d\n", b, (int)b->size);
#endif

    /* Work out new position and create the block */
    new_pos = b + sizeof(struct block) + (int)b->size;
    new = new_block(new_pos, b->size);
#if HEAP_DEBUG
    kprintf("split_block(): placed new block at %p (%d bytes)\n", new_pos, 
            (int)b->size);
#endif

    /* Stich the new block into the linked list */
    b->next = new;
    b->next->next = old_next;

#if HEAP_DEBUG
    /* Perform a sanity check on the heap to make sure we haven't corrupted
     * anything, and dump the heap.
     */
    heap_check(heap_start);
    dump_heap(heap_start);
#endif

    return new_pos;
}

void *new_heap(void *start, size_t size)
{
    assert(start != NULL);
    new_block(start, size);
    heap_start = start;
    return start;
}

void dump_heap(void *start)
{
    unsigned int i = 0;
    struct block *b = start;

    while (b != NULL) {
        kprintf("%d: %p\t%d\t", i, b, (int)b->size);
        if (b->free) {
            kprintf("free\n");
        } else {
            kprintf("used\n");
        }
        i++;
        b = b->next;
    }
    kprintf("\n");
}

void heap_check(void *start)
{
    struct block *b = start;
    while (b != NULL) {
        assert(b->magic == HEAP_MAGIC);
        assert(b->size <= HEAP_SIZE);
        b = b->next;
    }
}

void *alloc(size_t size)
{
    struct block *b = (struct block *)heap_start;

    while (b != NULL) {
        heap_check(heap_start);

        /* Ignore used blocks */
        if (!b->free) {
            b = b->next;
            continue;
        }

        /* Perfect match */
        if (b->size == size) {
#if HEAP_DEBUG
            kprintf("alloc(): found perfect fit at block %p for %d bytes\n", 
                    b, (int)size);
#endif
            b->free = 0;
            return DATA(b);
        /* Too small, try and coalesce */
        } else if (b->size < size) {
#if HEAP_DEBUG
            kprintf("alloc(): block %p too small, skipping\n", b);
#endif
        /* Too big, try and split the block */
        } else {
            /* If splitting the block would make it unusable to us, just
             * assume a best fit
             */
            int new_size = (b->size - sizeof(struct block)) / 2;
            if (new_size < (int)size) {
#if HEAP_DEBUG
                kprintf("alloc(): splitting %p would be too small "
                        "(%d bytes < %d bytes); assuming best fit\n", b,
                        new_size, (int)size);
                kprintf("alloc(): found best fit at block %p (%d bytes) for "
                        "%d bytes\n", b, (int)b->size, (int)size);
#endif
                b->free = 0;
                return DATA(b);
            } else {
                /* Otherwise, try and split the block */
                if (split_block(b) != NULL) {
                    /* continue so we check this block again */
                    continue;
                } else {
                    /* Splitting the block isn't possible, try and merge
                     * with the next block
                     */
#if HEAP_DEBUG
                    kprintf("XXX: merge %p and %p here\n", b, b->next);

                    /* Failing that, just use it */
                    kprintf("alloc(): found worst fit at block %p (%d bytes) for "
                            "%d bytes\n", b, (int)b->size, (int)size);
#endif
                    b->free = 0;
                    return DATA(b);
                }
            }
        }

        b = b->next;
    }
    
#if HEAP_DEBUG
    kprintf("alloc(): no suitable block found for %d bytes!!\n", (int)size);
#endif
    return NULL;
}
