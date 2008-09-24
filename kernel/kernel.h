
#ifndef _KERNEL_H
#define _KERNEL_H

/* This header sets up global definitions, and includes all the common header 
 * files required for kernel operation, to reduce clutter in the *.{c,h}
 * files.
 */

#define DEBUG 0 /* be verbose with what the kernel is doing - SLOW */

/* POSIX system includes */
#include <types.h>
#include <unistd.h>

/* Our own headers */
#include "kprintf.h"
#include "shutdown.h"

#endif

