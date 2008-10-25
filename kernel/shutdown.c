
/* kernel.h includes shutdown.h for us */
#include "kernel.h"

void panic(const char *msg)
{
    static int panicking = 0;
    if (panicking++) return; /* prevent recursive panics - thanks AST */

    kprintf_all("\nKernel panic: %s\n", msg);

#if __GNUC__
    /* GCC provides us with some nice debugging internals: dump them if built
     * with GCC.
     */
    kprintf_all("built with GCC; dumping stack addresses:\n");
    kprintf_all("\tlast function: %p\n", __builtin_return_address(1));
    kprintf_all("\tframe: %p\n", __builtin_frame_address(1));
#endif

    halt(); /* bye bye */
}

void shutdown(void)
{
    kprintf_all("Shutting down Ulysses\n");
    halt(); /* XXX but for now, just halt */
}

