#include <ulysses/syscall.h>

inline void sleep(unsigned long msecs)
{
    unsigned long start_time = msuptime();
    unsigned long finish_time = start_time + msecs;

    while (1) {
        register unsigned long curr_time = msuptime();
        if (curr_time >= finish_time) {
            break;
        }
    }
}
