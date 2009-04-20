#include <ulysses/cmos.h>
#include <ulysses/datetime.h>
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

struct datetime cmos_datetime(void)
{
    TRACE_ONCE;
    struct datetime t;
    t.year = BCD_INT(read_cmos(DT_YR));
    t.month = BCD_INT(read_cmos(DT_MTH));
    t.day = BCD_INT(read_cmos(DT_MDAY));
    t.hour = BCD_INT(read_cmos(DT_HR));
    t.min = BCD_INT(read_cmos(DT_MIN));
    t.sec = BCD_INT(read_cmos(DT_SEC));
    return t;
}
