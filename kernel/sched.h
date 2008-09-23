
#ifndef _SCHED_H
#define _SCHED_H

/* This is the kernel scheduler. The scheduler is currently a queue-based,
 * round-robin scheduler. It does *not* take uid into account.
 *
 * We declare the scheduling queues here. 0 is the highest priority, meaning
 * whichever process resides at the head of queue 0 is next to be executed on
 * the CPU.
 *
 * The lowest priority queue contains IDLE. If no processes are ready
 * in the higher queues, we fall back to the idle process and execute it.
 * IDLE *must* stay as the only process in the lowest priority queue!
 */

#include "proc.h"

#define SCHED_Q_NR 3 /* number of scheduling queues to use (IDLE_Q + 1) */
#define IDLE_Q 2 /* lowest queue, IDLE process always stays here */
#define USER_Q 1 /* user processes go here */
#define ROOT_Q 0 /* root's processes go here so they are picked first */
#define SCHED_QUANTUM 50 /* default quantum for processes */

#endif

