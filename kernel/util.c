
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
#if DEBUG
        kprintf("Kernel sanity check passed\n");
#endif
        return; /* so we don't have an empty label with DEBUG 0 */
}

void stub(char *fname)
{
#if DEBUG
    kprintf("stub: %s()\n", fname);
#endif
}

