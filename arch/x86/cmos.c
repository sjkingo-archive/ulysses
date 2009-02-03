
#include "cmos.h"
#include "util.h"
#include "../../kernel/kernel.h"

static unsigned char read_cmos(unsigned char reg)
{
    unsigned char v;
    outb(0x70, reg); /* open command port */
    v = inb(0x71); /* read from data port */
    outb(0x70, 0); /* close command port */
    return v;
}

void cmos_datetime(void)
{
    kprintf("Datetime from CMOS: %d-%d-%d %d:%d:%d\n", 
            BCD_INT(read_cmos(DT_YR)), BCD_INT(read_cmos(DT_MTH)), 
            BCD_INT(read_cmos(DT_MDAY)), BCD_INT(read_cmos(DT_HR)), 
            BCD_INT(read_cmos(DT_MIN)), BCD_INT(read_cmos(DT_SEC)));
}

