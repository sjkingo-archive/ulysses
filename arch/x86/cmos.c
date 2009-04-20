#include <ulysses/cmos.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <sys/time.h>
#include <sys/types.h>

static unsigned char read_cmos(unsigned char reg)
{
    TRACE_ONCE;
    unsigned char v;
    outb(0x70, reg); /* open command port */
    v = inb(0x71); /* read from data port */
    outb(0x70, 0); /* close command port */
    return v;
}

struct timeval cmos_datetime(void)
{
    TRACE_ONCE;
    struct timeval t;
    t.tv_sec = 0;
    return t;

#if 0
    kprintf("Datetime from CMOS: %d-%d-%d %d:%d:%d\n", 
            BCD_INT(read_cmos(DT_YR)), BCD_INT(read_cmos(DT_MTH)), 
            BCD_INT(read_cmos(DT_MDAY)), BCD_INT(read_cmos(DT_HR)), 
            BCD_INT(read_cmos(DT_MIN)), BCD_INT(read_cmos(DT_SEC)));
#endif
}
