
#ifndef _MUTEX_H
#define _MUTEX_H

/* spinlock_am()
 *  Adaptive mutex spinlock. Spin until bit is not set, but suspend thread
 *  when we yield.
 */
#define AM_THRESH 10
void spinlock_am(const unsigned char *bit);

#endif

