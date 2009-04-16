
#ifndef _x86_TIMER_H
#define _x86_TIMER_H

/* The CPU timer */

#include <ulysses/isr.h> /* registers_t */

#define CLOCK 1193180
#define TIMER_FREQ 50

unsigned long ticks;

void init_timer(unsigned int freq);

void timer_tick(registers_t regs);

#endif

