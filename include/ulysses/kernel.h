
#ifndef _KERNEL_H
#define _KERNEL_H

/* This sets up any global variables (pre-processor definitions should go
 * in ../config.h), and includes all common header files needed for kernel
 * operation - reducing clutter in the *.{c,h} files.
 */

/* Our own headers */
#include "../../config.h" /* must be first */
#include "../../arch/halt.h" /* halt code to fall back on */

#include <sys/time.h>
#include <ulysses/multiboot.h>

/* Ensure a valid architecture was declared */
#ifndef _ARCH_x86
#error No _ARCH_* defined!
#endif

/* The main kernel data structure */
struct kernel {
    multiboot_info_t *mbi; /* multiboot info struct as passed by loader */
    char *cmdline; /* kernel command line as passed by loader */

    char *cpu_vendor; /* VendorID of CPU */
    char *cpu_model; /* Model name of CPU */

    struct timeval loaded_time; /* time when kernel was first loaded */
    struct timeval current_time_offset; /* current time since loaded_time */
};
struct kernel kern;

/* startup_kernel()
 *  Perform higher-level kernel starup procedures. This should only be called
 *  after arch-specific startup has completed!
 */
void startup_kernel(void);

#endif

