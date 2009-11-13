#ifndef _ULYSSES_STATS_H
#define _ULYSSES_STATS_H

#include <sys/types.h>

struct statistics {
    flag_t in_use; /* whether this entry is being used */
    time_t cpu_time; /* total time spent on the CPU */
    unsigned long virtual_mem; /* bytes of virtual memory owned */
};

void new_stats(pid_t pid);
void destroy_stats(pid_t pid);

inline void stats_update_cpu_time(void);
inline time_t stats_get_cpu_time(pid_t pid);

inline void stats_update_virtual_mem(unsigned int bytes);
inline unsigned long stats_get_virtual_mem(pid_t pid);

#endif
