
#ifndef _KERNEL_H
#define _KERNEL_H

/* This header sets up global definitions, and includes all the common header 
 * files required for kernel operation, to reduce clutter in the *.{c,h}
 * files.
 */

#define DEBUG 1 /* be verbose with what the kernel is doing - SLOW */

/* POSIX system includes */
#include <types.h>
#include <unistd.h>

/* Our own headers */
#include "../config.h"
#include "kprintf.h"
#include "shutdown.h"

/* Arch-specific headers */
#ifdef _ARCH_x86
#include "../arch/x86/x86.h"
#else
#include "../arch/common/common.h" /* fall back to common - may not work! */
#endif

/* The main kernel data structure */
struct kernel {
    void *mdb; /* physical address of multiboot struct */
    unsigned int magic;
};
struct kernel kern;

/* sanity_check()
 *  Perform a sanity check of the kernel's data structures and physical memory
 *  locations. If anything fails the check, a panic() will be issued.
 */
void sanity_check(void);

#endif

