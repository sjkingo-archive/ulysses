
#include "kernel.h"
#include "proc.h"

static void print_memory_map(void)
{
    memory_map_t *mmap;
    
    if (!MB_FLAG(kern.mbi->flags, 0)) panic("No multiboot memory info");
    if (!MB_FLAG(kern.mbi->flags, 6)) panic("No multiboot memory map");

    kprintf("Lower memory: %u KB\nUpper memory: %u KB\n", kern.mbi->mem_lower,
                kern.mbi->mem_upper);
    kprintf("Memory map; addr %p, length %p\n", kern.mbi->mmap_addr, 
            kern.mbi->mmap_length);
    for (mmap = (memory_map_t *)kern.mbi->mmap_addr; (unsigned long)mmap < 
            (kern.mbi->mmap_addr + kern.mbi->mmap_length);
            mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + 
            sizeof(mmap->size))) {
        kprintf("  size %p, length = %p%x, type %p, base_addr %p%x\n",
                mmap->size, mmap->length_high, mmap->length_low, mmap->type,
                mmap->base_addr_high, mmap->base_addr_low);
    }
}

static void print_startup(void)
{
    kprintf("KOS - Kingo Operating System\n");
    kprintf("v0.1\t(Codename: Nomad)\n");
#ifdef _ARCH_x86
    kprintf("Compiled for x86\n");
#endif
}

void sanity_check(void)
{

    goto pass; /* default to passing */
    goto fail; /* to keep the compiler happy */
    fail:
        panic("Kernel sanity check failed");
    pass:
#if DEBUG
        kprintf("Kernel sanity check passed\n");
#endif
}

void _kmain(void *mdb, unsigned int magic)
{
    init_screen(); /* must be first in _kmain() */
    print_startup();

    /* Set up the multiboot stuff given to us by the boot loader */
    if (magic != MULTIBOOT_LOADER_MAGIC)
        panic("Kernel not booted by a Multiboot loader");
    else
        kprintf("Kernel booted by a Multiboot loader; magic %p\n", magic);
    kern.mbi = (multiboot_info_t *)mdb;
    print_memory_map();

    /* Set up the process table and scheduling queues and add IDLE as first
     * proc in table.
     */
    init_proc();
    new_proc(-1, -1, -1, "IDLE");
    proc[0].pid = PID_IDLE; /* XXX ignore the auto-generated pid */

    sanity_check(); /* do a sanity check of the kernel */

    /* Start IDLE going and never return */
    sched(get_proc(PID_IDLE));
    panic("_kmain() exited?");
}

