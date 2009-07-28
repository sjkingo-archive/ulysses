#ifndef _ULYSSES_TRACE_H
#define _ULYSSES_TRACE_H

/* This provides a function stacktrace for use in debugging, similar to gdb's
 * backtrace.
 */

#include <ulysses/stack.h>

/* A function symbol as an element of a linked list */
typedef struct symbol {
    const char *name;
    void *addr;
    struct symbol *prev;
    struct symbol *next;
} symbol_t;

/* get_closest_symbol()
 *  Find the function symbol that this address is offset into.
 */
symbol_t *get_closest_symbol(void *addr);

/* get_trace_symbol()
 *  Find the function given by func_name and return its symbol.
 */
symbol_t *get_trace_symbol(const char *func_name);

/* add_trace_symbol()
 *  Expands from TRACE_ONCE macro to register a function as a symbol.
 *  Don't call this manually unless you really have a reason to do so.
 */
void add_trace_symbol(const char func_name[], void *addr);

/* dump_syms()
 *  Output all of the symbols currently recorded.
 */
void dump_syms(void);

#endif
