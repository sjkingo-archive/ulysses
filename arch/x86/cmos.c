/* arch/x86/cmos.c - provides access to the onboard CMOS chip
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/cmos.h>
#include <ulysses/datetime.h>
#include <ulysses/util.h>

#include <sys/time.h>
#include <sys/types.h>

static unsigned char read_cmos(unsigned char reg)
{
    unsigned char v;
    outb(0x70, reg); /* open command port */
    v = inb(0x71); /* read from data port */
    outb(0x70, 0); /* close command port */
    return v;
}

struct datetime cmos_datetime(void)
{
    struct datetime t;
    t.year = BCD_INT(read_cmos(DT_YR));
    t.month = BCD_INT(read_cmos(DT_MTH));
    t.day = BCD_INT(read_cmos(DT_MDAY));
    t.hour = BCD_INT(read_cmos(DT_HR));
    t.min = BCD_INT(read_cmos(DT_MIN));
    t.sec = BCD_INT(read_cmos(DT_SEC));
    return t;
}
