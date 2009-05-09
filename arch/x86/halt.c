/* This is the x86-specific halt code. First we try to disable interrupts
 * and halt the CPU. Fall back to a busy loop if that didn't work.
 */

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/util.h>

void halt(void)
{
    CLI; /* since we're about to die anyway */
    kprintf("\nKernel uptime %d seconds\n", kern.current_time_offset.tv_sec);
    kprintf("CPU halt\n");
    while (1) __asm__ __volatile__("hlt");

    /* Wow, the CPU on this machine must really be stuffed. Enter a busy
     * loop instead.
     */
    kprintf("\nCPU woken from halt; You must be Ken Thompson...\n");
    kprintf("\"I mean, if 10 years from now, when you are doing something "
            "quick and dirty,\nyou suddenly visualize that I am looking over "
            "your shoulders and say to\nyourself \'Dijkstra would not have "
            "liked this\', well, that would be enough\nimmortality for "
            "me.\"\n");
    kprintf("\t-- Edsger W. Dijkstra, 30 August 1995\n");
    while (1);
}
