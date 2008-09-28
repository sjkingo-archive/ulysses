
/* kernel.h includes shutdown.h for us */
#include "kernel.h"

/* Internal function prototypes */
void halt(void);

void halt(void)
{
    kprintf("\nCPU halted");

    /* Disable interrupts and halt. CPU shouldn't wake up from this... */
    __asm__("cli");
    __asm__("hlt");

#if DEBUG
    kprintf("\nCPU woken from halt, suiciding instead...");
#endif

    /* ... however, if it was... last ditch effort to halt the CPU - 
     * generate a triple fault by referencing an invalid memory location. */
    void (*suicide)(void);
    suicide = (void (*)(void)) - 1;
    suicide();
}

void panic(const char *msg)
{
    static int panicking = 0;
    if (panicking++) return; /* prevent recursive panics - thanks AST */

    kprintf("\nKernel panic: %s\n", msg);

#if __GNUC__
    /* GCC provides us with some nice debugging internals: dump them if built
     * with GCC.
     */
    kprintf("built with GCC; dumping stack addresses:\n");
    kprintf("\tlast function: %p\n", __builtin_return_address(1));
    kprintf("\tframe: %p\n", __builtin_frame_address(1));
#endif

    halt(); /* bye bye */
}

void shutdown(void)
{
    kprintf("Shutting down KOS\n");
    halt(); /* XXX but for now, just halt */
}

