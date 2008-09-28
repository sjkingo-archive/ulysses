
#ifndef _x86_HALT_H
#define _x86_HALT_H

/* This is the x86-specific halt code. First we try to disable interrupts
 * and halt the CPU. Fall back to suiciding if that doesn't work.
 */

#include "../common/suicide.h"

#define halt(); \
        kprintf("\nCPU halted"); \
        __asm__ __volatile__("cli ; hlt"); \
        kprintf("\nCPU woken from halt, suiciding instead..."); \
        halt_suicide();

#endif

