
#include "mutex.h"
#include "yield.h"

void spinlock_am(const unsigned char *bit)
{
    register unsigned int spins = 0;
    while (!(*bit)) { /* waste some CPU */
        if ((++spins) > AM_THRESH) { yield();
    }
}

