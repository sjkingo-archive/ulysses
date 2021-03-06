#ifndef _TYPES_H
#define _TYPES_H

#ifndef NULL
#define NULL ((void *)0)
#endif

#define TRUE 1
#define FALSE 0

typedef int ssize_t;
typedef unsigned int size_t;

typedef int uid_t; /* user id */
typedef int gid_t; /* group id */
typedef int pid_t; /* process id */
typedef unsigned char flag_t; /* 0/1 logical flag */

typedef long time_t; /* time in sec since epoch */
typedef unsigned long mseconds_t; /* time in milliseconds */
typedef unsigned long useconds_t; /* time in microseconds (unsigned) */
typedef long suseconds_t; /* time in microseconds (signed) */

#endif

