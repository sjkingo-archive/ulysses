
/* kernel.h includes shutdown.h for us */
#include "kernel.h"

/* halt()
 *  Attempt to halt the CPU disabling all interrupts and halting it. If this
 *  fails, jump to an invalid memory location to trigger a triple fault.
 */
void halt(void)
{
    __asm__("cli");
    __asm__("hlt");

    /* Last ditch effort to halt the CPU - generate a triple fault */
    void (*suicide)(void);
    suicide = (void (*)(void)) - 1;
    suicide();
}

/* panic()
 *  Output a panic message and halt the CPU.
 */
void panic(const char *msg)
{
    static int panicking = 0;
    if (panicking++) return; /* prevent recursive panics - thanks AST */
    if (msg != NULL) kprintf("Kernel panic: %s", msg);
    halt();
}

/* shutdown()
 *  Properly shut down KOS using ACPI.
 */
void shutdown(void)
{
    /* XXX but for now, just halt() */
    halt();
}

