#ifndef _ULYSSES_CPU_H
#define _ULYSSES_CPU_H

/* cpu_ticks()
 *  Get the number of CPU ticks since the last processor reset. We do this in
 *  assembler to be as efficient as possible. Note that some overhead exists in
 *  merging the two 32-bit registers. Returns an unsigned long long.
 */
#define cpu_ticks() \
({ \
    unsigned int a, d; \
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d)); \
    ((unsigned long long)a | ((unsigned long long)d << 32)); \
})

/* idle_cpu()
 *  Loop forever in S1 sleep state. Set interrupts before halting the CPU
 *  to ensure the CPU has active interrupt lines.
 */
void idle_cpu(void);

/* dump_regs()
 *  Print the values stored in the CPU registers. This includes:
 *    ebp, esp
 *    eax, ebx, ecx, edx
 *    edi and esi
 */
void dump_regs(void);

#endif
