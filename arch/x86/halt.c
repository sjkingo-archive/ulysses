/* This is the x86-specific halt code. First we try to disable interrupts
 * and halt the CPU. Fall back to suiciding if that doesn't work.
 */

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/util.h>

#include "../halt.h"

void halt(void)
{
    CLI;
    kprintf("\nKernel uptime %d seconds\n", kern.current_time_offset.tv_sec);
    kprintf("CPU halt\n");
    while (1) __asm__ __volatile__("hlt");
    kprintf("\nCPU woken from halt, suiciding instead...");
    HALT_SUICIDE;
}
