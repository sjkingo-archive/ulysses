
#include "paging.h"
#include "../../kernel/kernel.h"

#include <string.h> /* for memset() */
#include <types.h> /* for flag_t */

extern unsigned int end; /* end of kernel: declared in linker.ld */

unsigned int kmalloc(unsigned int size, flag_t align, unsigned int *phys)
{
    /* Perform alignment */
    if (align && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    /* Give a physical address */
    if (phys) *phys = placement_address;

    /* Allocate the required amount */
    unsigned int start = placement_address;
    placement_address += size;
    return start;
}

static void set_frame(unsigned int frame_addr)
{
    unsigned int frame = frame_addr / 0x1000;
    frames[INDEX_FROM_BIT(frame)] |= (0x1 << OFFSET_FROM_BIT(frame));
}

static void clear_frame(unsigned int frame_addr)
{
    unsigned int frame = frame_addr / 0x1000;
    frames[INDEX_FROM_BIT(frame)] &= ~(0x1 << OFFSET_FROM_BIT(frame));
}

static unsigned int first_frame(void)
{
    unsigned int i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] == 0xFFFFFFFF) continue; /* nothing free */
        for (j = 0; j < 32; j++) {
            if (!(frames[i] & (0x1 << j))) return (i * 32 + j);
        }
    }
    return -1; /* no free frames */
}

static void switch_page_dir(page_dir_t *dir)
{
    unsigned int cr0;
    current_directory = dir;
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (&dir->tables_phys));
    __asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("mov %0, %%cr0" : : "r" (cr0));
}

static page_t *get_page(unsigned int addr, flag_t make, page_dir_t *dir)
{
    unsigned int index = addr / 0x1000;
    unsigned int table_index = index / 1024;

    /* If the table is already assigned */
    if (dir->tables[table_index])
        return (&dir->tables[table_index]->pages[index % 1024]);

    /* Since it isn't already assigned, create and assign it */
    if (make) {
        unsigned int tmp;
        dir->tables[table_index] = 
                    (page_table_t *)kmalloc(sizeof(page_table_t), 1, &tmp);
        memset(dir->tables[table_index], 0, 0x1000);
        dir->tables_phys[table_index] = tmp | 0x7; /* present, rw, us */
        return (&dir->tables[table_index]->pages[index % 1024]);
    }

    return 0; /* did nothing */
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    unsigned int index; /* index of first free frame */

    if (page->frame != 0) return; /* already allocated */
    
    if ((index = first_frame()) == (unsigned int)-1)
        panic("No free frames in page table");
    
    set_frame(index * 0x1000);
    page->present = 1;
    page->rw = is_writeable;
    page->user = !is_kernel;
    page->frame = index;
}

void free_frame(page_t *page)
{
    unsigned int frame;
    if (!(frame = page->frame)) return; /* no allocated frames in page */
    clear_frame(frame);
    page->frame = 0x0;
}

flag_t init_paging(void)
{
    unsigned int i = 0;

    /* Set up page frames */
    placement_address = (unsigned int)&end;
    //nframes = kern.mbi->mem_upper / 0x1000;
    nframes = 0x500000 / 0x1000; /* XXX 8 MB */
    frames = (unsigned int *)kmalloc(INDEX_FROM_BIT(nframes), 0, NULL);
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    /* Set up a page directory */
    kernel_directory = (page_dir_t *)kmalloc(sizeof(page_dir_t), 1, NULL);
    memset(kernel_directory, 0, sizeof(page_dir_t));
    current_directory = kernel_directory;
    
    /* Set kernel memory to readable but not writeable to user-space */
    while (i < placement_address) {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    /* Register a page fault handler and enable paging */
    register_interrupt_handler(14, &page_fault);
    switch_page_dir(kernel_directory);

    return TRUE; /* clean return for kmain() */
}

void page_fault(registers_t regs)
{
    /* Get the faulting address from the CR2 register */
    unsigned int faulting_addr;
    __asm__ __volatile("mov %%cr2, %0" : "=r" (faulting_addr));

    /* Print some info */
    kprintf("Page fault at %x with error %d\n", faulting_addr, regs.err_code);
    if (!(regs.err_code & 0x1)) kprintf("\tpage not present \n");
    if (regs.err_code & 0x2) kprintf("\twrite operation\n");
    if (regs.err_code & 0x4) kprintf("\t!ring 0\n");
    if (regs.err_code & 0x8) kprintf("\tcorrupted page entry\n");
    if (regs.err_code & 0x10) kprintf("\tcaused by instruction fetch\n");

    /* Panic :-( */
    panic("Page fault");
}

