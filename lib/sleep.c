#include <ulysses/util.h>

inline void sleep(unsigned long msecs)
{
    unsigned long start_time = do_msuptime();
    unsigned long finish_time = start_time + msecs;

    while (1) {
        register unsigned long curr_time = do_msuptime();
        if (curr_time >= finish_time) {
            break;
        }
    }
}
