
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

void _kmain(void *mdb, unsigned int magic)
{
#ifdef _ARCH_x86
    startup_x86(mdb, magic);
#endif
    
    /* Set up the virtual terminals - this will switch us to the LOG VT */
    if (!init_vt()) panic("Virtual terminals failed initialisation");
    print_startup();

#if DEBUG
    print_memory_map(); /* print memory map given by multiboot */
#endif

    //init_initrd(*(unsigned int *)kern.mbi->mods_addr); /* set up root fs */
    init_proc(); /* set up process table and scheduling queues */
    sanity_check(); /* do a sanity check of the kernel */

    /* Force IDLE going and never return */
    idle_task(); /* XXX */
}

