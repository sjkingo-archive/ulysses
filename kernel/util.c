
#include <ulysses/kernel.h>
#include <ulysses/util.h>
#include <ulysses/kheap.h>

static void print_memory_map(void)
{
    memory_map_t *mmap;

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

/* just kidding Dijkstra */
#define COMEFROM goto
void sanity_check(void)
{
    /* maybe not */
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
    kprintf("stub: %s()\n", fname);
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

void print_cpuinfo(void)
{
    kprintf("Detected 1 CPU(s): %s %s\n", kern.cpu_vendor, kern.cpu_model);
}

void print_meminfo(void)
{
    print_memory_map();
    kprintf("Kernel heap at %p\n", KHEAP_START);
}

