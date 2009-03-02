
#include "kernel.h"

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
        kprintf("\tsize %p\tlength %p%x\ttype %x\tbase_addr %p%x\n",
                mmap->size, mmap->length_high, mmap->length_low, mmap->type,
                mmap->base_addr_high, mmap->base_addr_low);
    }
}

void startup_kernel(void)
{
    static flag_t already_started = FALSE;
    if (already_started) {
        panic("startup_kernel() already set up");
    }
    
    if (!init_vt()) panic("Virtual terminals failed initialisation");
    print_startup();
    print_cpuinfo();
    print_memory_map(); /* print memory map given by multiboot */
    init_proc(); /* set up process table and scheduling queues */
    init_initrd(*(unsigned int *)kern.mbi->mods_addr); /* set up root fs */
    init_shell("ulysses> "); /* this happens regardless of KERN_SHELL's val */

    already_started = TRUE;
    kprintf("Kernel startup complete in %ds\n", 
            kern.current_time_offset.tv_sec);

#if KERN_SHELL
    run_shell(); /* display shell */
#endif
}

void _kmain(void *mdb, unsigned int magic)
{
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif

#if KERN_INTERACTIVE
    print_startup();
    kprintf("Compiled with KERN_INTERATIVE, halting startup\n");
    init_shell("ulysses> ");
    run_shell();
#else
    startup_kernel();
#endif
}

