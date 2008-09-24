
#include "kprintf.h"

#include <unistd.h>

/* halt()
 *  Attempt to halt the CPU by jumping to an invalid memory location, or if
 *  this fails, disable all interrupts and halt the CPU.
 */
void halt(void)
{
    void (*suicide)(void);
    suicide = (void (*)(void)) - 1;
    suicide();

    /* If we got here, suicide didn't work (eh?), so drop into a CPU halt */
    __asm__("cli");
    __asm__("hlt");
}

/* panic()
 *  Output a panic message and halt the CPU.
 */
void panic(const char *msg)
{
    static int panicking = 0;
    if (panicking++) return; /* prevent recursive panics - thanks AST */

    if (msg != NULL) kprintf("\nKernel panic: %s\n", msg);
    halt();
}

