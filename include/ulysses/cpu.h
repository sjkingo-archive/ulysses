#ifndef _ULYSSES_CPU_H
#define _ULYSSES_CPU_H

/* idle_cpu()
 *  Loop forever in S1 sleep state. Set interrupts before halting the CPU
 *  to ensure the CPU has active interrupt lines.
 */
void idle_cpu(void);

#endif
