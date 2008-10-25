
/* This is the x86-specific halt code. First we try to disable interrupts
 * and halt the CPU. Fall back to suiciding if that doesn't work.
 */

#include "x86.h"
#include "../halt.h"

void halt(void)
{
    kprintf("\nCPU halt");
    __asm__ __volatile__("cli ; hlt");
    kprintf("\nCPU woken from halt, suiciding instead...");
    HALT_SUICIDE;
}

