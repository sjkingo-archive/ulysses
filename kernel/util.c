
#include "kernel.h"
#include "util.h"

void sanity_check(void)
{
    stub((char *)__func__);

    goto pass; /* default to passing */
    goto fail; /* to keep the compiler happy */
    fail:
        panic("Kernel sanity check failed");
    pass:
        return; /* so we don't have an empty label with DEBUG 0 */
}

void stub(char *fname)
{
#if 0
    kprintf("stub: %s()\n", fname);
#endif
}

void print_startup(void)
{
    kprintf("Ulysses\tv%s (codename: %s)\n", VERSION_NUM, VERSION_CN);
    kprintf("Kernel command line: '%s'\n", kern.cmdline);
#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif
#ifdef __GNUC__
    kprintf("Compiled by gcc %s\n", __VERSION__);
#endif
#ifdef __LP64__
    kprintf("Compiled with 64-bit long int support\n");
#endif
#ifdef __STRICT_ANSI__
    kprintf("Warning: compiled with -ansi\n");
#endif
#ifdef __OPTIMIZE__
    kprintf("Warning: compiled with -Ox optimisations; expect limited "
            "debugging capacity\n");
#endif
}

