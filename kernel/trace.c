#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/trace.h>

#include <string.h>

typedef struct symbol {
    const char *name;
    void *addr;
    struct symbol *next;
} symbol_t;

symbol_t *head = NULL;
symbol_t *tail = NULL;

symbol_t *lookup_symbol(void *addr)
{
    symbol_t *s = head;
    while (s != NULL) {
        if (s->addr == addr) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

void add_trace_symbol(const char func_name[], void *addr)
{
    /* Don't add duplicate symbols */
    if (lookup_symbol(addr) != NULL) {
        return;
    }

    symbol_t *s = kmalloc(sizeof(symbol_t));
    s->name = func_name;
    s->addr = addr;
    s->next = NULL;

    /* Add to list */
    if (head == NULL) {
        head = s;
    } else if (tail == NULL) {
        tail = s;
        head->next = s;
    } else {
        tail->next = s;
        tail = s;
    }
}

void func_trace(unsigned int max_frames)
{
    TRACE_ONCE;
    unsigned int i, *ebp;

    kprintf("Function call trace (from top of stack to bottom):\n");

    ebp = &max_frames - 2;
    for (i = 0; i < max_frames; ++i) {
        unsigned int eip;
        symbol_t *sym;

        /* Make sure we don't walk over the end of the stack */
        eip = ebp[1];
        if (eip == 0) {
            break;
        }

        /* Lookup the symbol */
        sym = lookup_symbol(eip);
        if (sym == NULL) {
            kprintf("\t%d: %p\n", i, eip);
        } else {
            kprintf("\t%d: %p (%s)\n", i, eip, sym->name);
        }

        ebp = (unsigned int *)ebp[0]; /* unwind to previous */
    }
}
