
#ifndef _x86_TIMER_H
#define _x86_TIMER_H

/* The CPU timer */

#define CLOCK 1193180
#define TIMER_FREQ 250

unsigned long ticks;

flag_t init_timer(unsigned int freq);

void timer_tick(registers_t regs);

#endif

