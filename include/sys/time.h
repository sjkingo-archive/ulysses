
#ifndef _TIME_H
#define _TIME_H

#include <sys/types.h>

struct timeval {
    time_t tv_sec; /* seconds */
    suseconds_t tv_usec; /* microseconds */
};

struct itimerval {
    struct timeval it_interval; /* timer interval */
    struct timeval it_value; /* current value */
};

#endif

