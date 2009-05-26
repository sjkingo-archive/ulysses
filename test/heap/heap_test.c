#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../../include/ulysses/heap.h"

#define HEAP_SIZE 1024

void *heap_start = NULL;

/* heap.o needs these symbols */
void kprintf(const char *fmt, ...);
void do_panic(const char *msg, const char *file, int line);

void kprintf(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    vprintf(fmt, argp);
    va_end(argp);
}

void do_panic(const char *msg, const char *file, int line)
{
    msg = NULL;
    file = NULL;
    line = 0;
    abort();
}

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
