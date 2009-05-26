/* test/heap/main.c - heap tester
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../../include/ulysses/heap.h"

#define HEAP_SIZE 1024

void *heap_start = NULL;

void test_allocate(size_t bytes)
{
    void *ptr = alloc(bytes);
    if (ptr == NULL) {
        printf("Got back NULL pointer for %d bytes\n", (int)bytes);
    } else {
        printf("alloc() gave us block at %p for %d bytes\n", ptr, (int)bytes);
    }
    dump_heap(heap_start);
}

int main(void)
{
    printf("sizeof(struct block) = %d\n", (int)sizeof(struct block));
    heap_start = new_heap(malloc(HEAP_SIZE), HEAP_SIZE);
    printf("heap_start = %p\n", heap_start);
    
    test_allocate(34);
    test_allocate(256);
    test_allocate(10);
    test_allocate(1);
    test_allocate(6);
    test_allocate(23);

    return 0;
}
