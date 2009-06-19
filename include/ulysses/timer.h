#ifndef _ULYSSES_TIMER_H
#define _ULYSSES_TIMER_H

/* The CPU timer */

#include <ulysses/isr.h> /* registers_t */

/* TIMER_FREQ
 *  Frequency to run the system timer at, in hertz. The higher the frequency,
 *  the more often the timer will tick, and the operating system will be more
 *  responsive. The frequency range of the PIT is fixed at [18..1000] Hz.
 *
 *  1000 Hz = every 1 ms (highest possible)
 *  250 Hz  = every 4 ms
 *  100 Hz  = every 10 ms
 *  50 Hz   = every 50 ms
 *  18 Hz   = every 55 ms (lowest possible)
 */
#define TIMER_FREQ 1000

#define CLOCK 1193180

/* init_timer()
 *  Activate the timer on IRQ0.
 */
void init_timer(unsigned int freq);

/* timer_tick()
 *  Interrupt vector for a timer tick.
 */
void timer_tick(registers_t *regs);

#endif
