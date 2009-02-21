
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
#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif
    kprintf("%d seconds since load\n", kern.current_time_offset.tv_sec);
}

