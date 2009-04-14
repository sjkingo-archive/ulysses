
#ifndef _HALT_H
#define _HALT_H

/* halt()
 *  Stops the CPU. This function never returns.
 */
void halt(void);

/* HALT_SUICIDE is a macro to be called in an emergency, as a last-resort 
 * means of stopping the CPU. Its definition is such that it will 
 * stop (or break) on any imaginable system.
 *
 * Since we don't know what halt instructions exist on every arch,
 * we force the CPU to generate a triple fault. This will halt it, not
 * very nicely mind you...
 */
#define HALT_SUICIDE \
    do { \
        kprintf("\nCPU halt (via suicide)\n"); \
        void (*suicide)(void); \
        suicide = (void (*)(void)) - 1; \
        suicide(); \
    } while (0)

#endif

