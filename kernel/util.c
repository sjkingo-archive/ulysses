#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/multiboot.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include "../config.h"

/* just kidding Dijkstra */
#define COMEFROM goto
void sanity_check(void)
{
    TRACE_ONCE;

    /* ... maybe not */
#ifdef COMEFROM
    COMEFROM pass; /* default to passing */
    COMEFROM fail; /* to keep the compiler happy */
    fail:
        panic("Kernel sanity check failed");
    pass:
#endif
        return; /* so we don't have an empty label with DEBUG 0 */
}

void stub(char *fname)
{
    TRACE_ONCE;
    kprintf("stub: %s()\n", fname);
}

void print_startup(void)
{
    TRACE_ONCE;
    kprintf("Ulysses\tv%s (codename: %s)\n", VERSION_NUM, VERSION_CN);
    kprintf("Kernel command line: '%s'\n", kern.cmdline);

#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif

#ifdef __GNUC__
    kprintf("Compiled by gcc %s\n", __VERSION__);
#else
    kprintf("Warning: not compiled by gcc; this disables a lot of nice "
            "debugging symbols\n");
#endif

#ifdef __LP64__
    kprintf("Compiled with 64-bit long int support\n");
#endif
#ifdef __STRICT_ANSI__
    kprintf("Warning: compiled with -ansi (how did that even work?)\n");
#endif
#ifdef __OPTIMIZE__
    kprintf("Warning: compiled with -Ox optimisations; expect limited "
            "debugging capacity\n");
#endif

    kprintf("CMOS-provided time %d-%d-%d %d:%d:%d (no timezone; "
            "probably UTC)\n", 
            kern.startup_datetime.year, kern.startup_datetime.month,
            kern.startup_datetime.day, kern.startup_datetime.hour,
            kern.startup_datetime.min, kern.startup_datetime.sec);
    kprintf("Kernel heap located at %p\n", KHEAP_START);
    kprintf("Kernel stack located at %p\n", STACK_LOC);
    kprintf("Detected %u KB of lower and %u KB of upper memory\n", 
            kern.mbi->mem_lower, kern.mbi->mem_upper);
    kprintf("Detected 1 CPU(s): %s %s\n", kern.cpu_vendor, kern.cpu_model);
}
