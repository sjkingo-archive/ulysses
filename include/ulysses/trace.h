#ifndef _ULYSSES_STACKTRACE_H
#define _ULYSSES_STACKTRACE_H

/* This provides a function stacktrace for use in debugging, similar to gdb's
 * backtrace.
 */

/* A function symbol as an element of a linked list */
typedef struct symbol {
    const char *name;
    void *addr;
    struct symbol *next;
} symbol_t;

/* This only works if the kernel was compiled with GCC; don't add trace
 * symbols if another compiler was used.
 */
#if __GNUC__
#define TRACE_ONCE add_trace_symbol(__func__, __builtin_return_address(0))
#else
#define TRACE_ONCE do {} while(0)
#endif

/* lookup_symbol()
 *  Translate the given memory address to a function name and return its
 *  symbol.
 */
symbol_t *lookup_symbol(void *addr);

/* get_trace_symbol()
 *  Find the function given by func_name and return its symbol.
 */
symbol_t *get_trace_symbol(const char *func_name);

/* add_trace_symbol()
 *  Expands from TRACE_ONCE macro to register a function as a symbol.
 *  Don't call this manually unless you really have a reason to do so.
 */
void add_trace_symbol(const char func_name[], void *addr);

/* func_trace()
 *  Output a function traceback right back to the kernel's entry point.
 */
void func_trace(unsigned int max_frames);

#endif
