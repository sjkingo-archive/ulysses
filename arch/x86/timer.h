
#ifndef _TIMER_H
#define _TIMER_H

/* The CPU timer */

#define CLOCK 1193180

unsigned long ticks;

void init_timer(unsigned int freq);

/* Declared in main.c */
extern void timer_tick(unsigned long ticks);

#endif

