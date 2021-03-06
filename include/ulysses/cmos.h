#ifndef _ULYSSES_CMOS_H
#define _ULYSSES_CMOS_H

/* CMOS-specific functions.
 * This module provides access to the CMOS clock for the chip's datetime.
 */

#include <ulysses/datetime.h>

#include <sys/time.h>

/* CMOS indexes for datetime */
#define DT_SEC 0
#define DT_MIN 2
#define DT_HR 4
#define DT_WDAY 6
#define DT_MDAY 7
#define DT_MTH 8
#define DT_YR 9

/* cmos_datetime()
 *  Returns the current CMOS timestamp in a datetime struct.
 */
struct datetime cmos_datetime(void);

#endif
