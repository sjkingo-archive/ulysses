#ifndef _ULYSSES_TIMER_H
#define _ULYSSES_TIMER_H

/* The CPU timer */

#include <ulysses/isr.h> /* registers_t */

#define CLOCK 1193180
#define TIMER_FREQ 50

unsigned long ticks;

/* init_timer()
 *  Activate the timer on IRQ0.
 */
void init_timer(unsigned int freq);

/* timer_tick()
 *  Interrupt vector for a timer tick.
 */
void timer_tick(registers_t regs);

#endif
