#include "../config.h"
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>

void do_panic(const char *msg, const char *file, int line)
{
    TRACE_ONCE;
    static int panicking = 0;
    if (panicking++) halt(); /* prevent recursive panics - thanks AST */
    
    kprintf_all("\n\nKernel panic: %s\n", msg);

#if __GNUC__
    /* Try and work out where the panic came from */
    void *source_addr = __builtin_return_address(1);
    symbol_t *sym = lookup_symbol(source_addr);

    if (sym == NULL) {
        kprintf_all("%p in ??? () ", source_addr);
    } else {
        kprintf_all("%p in %s () ", source_addr, sym->name);
    }
#else
    kprintf_all("??? in ??? () ");
#endif
    
    kprintf_all("at %s:%d\n\n", file, line);

    /* Dump a function call trace to screen for use in debugging */
    func_trace(50);

    halt(); /* bye bye */
}

void shutdown(void)
{
    TRACE_ONCE;
    kprintf_all("Shutting down Ulysses\n");
    kprintf_all("Killing all tasks...\n");
    kill_all_tasks();
    /* XXX allow the scheduler to actually remove the tasks? */
    halt(); /* XXX but for now, just halt */
}
