#ifndef _ULYSSES_KERNEL_H
#define _ULYSSES_KERNEL_H

/* This sets up any global variables (pre-processor definitions should go
 * in /config.h instead) for the kernel.
 */

/* Our own headers */
#include "../../config.h" /* must be first */

#include <ulysses/datetime.h>
#include <ulysses/multiboot.h>

#include <sys/time.h>

/* Ensure a valid architecture was declared */
#ifndef _ARCH_x86
#error No _ARCH_* defined!
#endif

/* Kernel stack is 8 KB */
#define STACK_LOC 0xE0000000
#define STACK_SIZE 0x2000

/* The main kernel data structure */
struct kernel {
    multiboot_info_t *mbi; /* multiboot info struct as passed by loader */
    char *cmdline; /* kernel command line as passed by loader */

    char *cpu_vendor; /* VendorID of CPU */
    char *cpu_model; /* Model name of CPU */
    unsigned int cpu_freq; /* Frequency in MHz of CPU */

    struct datetime startup_datetime;
    struct timeval loaded_time; /* time when kernel was first loaded */
    struct timeval current_time_offset; /* current time since loaded_time */
};
struct kernel kern;

#endif
