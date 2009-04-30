#ifndef _ULYSSES_DATETIME_H
#define _ULYSSES_DATETIME_H

/* Datetime definitions that do not exist in POSIX's <sys/time.h>. */

struct datetime {
  unsigned int year;
  unsigned int month;
  unsigned int day;
  unsigned int hour;
  unsigned int min;
  unsigned int sec;
};

#endif
