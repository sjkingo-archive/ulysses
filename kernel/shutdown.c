#include "../config.h"
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>

#include "../arch/halt.h"

void do_panic(const char *msg, const char *file, int line)
{
    TRACE_ONCE;
    static int panicking = 0;
    if (panicking++) halt(); /* prevent recursive panics - thanks AST */

    kprintf_all("\n\nKernel panic: %s\n", msg);
    kprintf_all("called from %s:%d\n", file, line);

#if __GNUC__
    /* GCC provides us with some nice debugging internals: dump them if built
     * with GCC.
     */
    kprintf_all("built with GCC; dumping stack addresses:\n");
    kprintf_all("\tlast function: %p\n", __builtin_return_address(1));
    kprintf_all("\tframe: %p\n", __builtin_frame_address(1));
#endif

    /* Dump a function call trace to screen for use in debugging */
    func_trace(50);

    halt(); /* bye bye */
}

void shutdown(void)
{
    TRACE_ONCE;
    kprintf_all("Shutting down Ulysses\n");
    halt(); /* XXX but for now, just halt */
}
