#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>

#include "../arch/halt.h"

void do_panic(const char *msg, const char *file, int line)
{
    static int panicking = 0;
    if (panicking++) return; /* prevent recursive panics - thanks AST */

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

    halt(); /* bye bye */
}

void shutdown(void)
{
    kprintf_all("Shutting down Ulysses\n");
    halt(); /* XXX but for now, just halt */
}
