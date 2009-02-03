
#ifndef _KERNEL_H
#define _KERNEL_H

/* This sets up any global variables (pre-processor definitions should go
 * in ../config.h), and includes all common header files needed for kernel
 * operation - reducing clutter in the *.{c,h} files.
 */

/* Our own headers */
#include "../config.h" /* must be first */
#include "../arch/halt.h" /* halt code to fall back on */
#include "initrd.h"
#include "kprintf.h"
#include "proc.h"
#include "shutdown.h"
#include "util.h"
#include "vt.h"

/* Arch-specific headers */
#ifdef _ARCH_x86
#include "../arch/x86/x86.h"
#else
#error No _ARCH_* defined!
#endif

/* POSIX system includes */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* The main kernel data structure */
struct kernel {
    multiboot_info_t *mbi; /* multiboot info struct as passed by loader */
    struct timeval loaded_time; /* time when kernel was first loaded */
    struct timeval current_time_offset; /* current time since loaded_time */
};
struct kernel kern;

#endif

