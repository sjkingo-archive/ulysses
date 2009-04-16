#include <ulysses/kheap.h>
#include <ulysses/paging.h>
#include <ulysses/shutdown.h>

#include <string.h> /* for memset() */

unsigned int *frames; /* pointer to first frame */
unsigned int nframes; /* number of frames */

page_dir_t *kernel_directory = 0; /* kernel's page directory */
page_dir_t *current_directory = 0; /* current page directory */

/* declared in kheap.c */
extern unsigned int placement_address;
extern heap_t *kheap;

extern unsigned int initial_esp; /* main.c */
extern void copy_page_physical(unsigned int, unsigned int); /* task.s */

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

/* first_frame()
 *  Find the first free page frame and return its index.
 */
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

/* switch_page_dir()
 *  Change the current page directory on the CPU.
 */
static void switch_page_dir(page_dir_t *dir)
{
    unsigned int cr0;
    current_directory = dir;
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (dir->phys_addr));
    __asm__ __volatile__("mov %%cr0, %0" : "=r" (cr0));
    cr0 |= 0x80000000; /* enable paging */
    __asm__ __volatile__("mov %0, %%cr0" : : "r" (cr0));
}

static page_table_t *clone_table(page_table_t *src, unsigned int *phys_addr)
{
    page_table_t *table;
    int i;

    table = (page_table_t *)kmalloc_ap(sizeof(page_table_t), phys_addr);
    memset(table, 0, sizeof(page_table_t));

    for (i = 0; i < 1024; i++) {
        if (src->pages[i].frame) {
            alloc_frame(&table->pages[i], 0, 0);
            if (src->pages[i].present) table->pages[i].present = 1;
            if (src->pages[i].rw) table->pages[i].rw = 1;
            if (src->pages[i].user) table->pages[i].user = 1;
            if (src->pages[i].accessed) table->pages[i].accessed = 1;
            if (src->pages[i].dirty) table->pages[i].dirty = 1;
            copy_page_physical(src->pages[i].frame * 0x1000, table->pages[i].frame * 0x1000);
        }
    }

    return table;
}

/* get_page()
 *  Return the page entry matching the given *virtual* address from page
 *  directory dir. If the address is not assigned to a page entry and make
 *  is set, create and assign the entry.
 */
page_t *get_page(unsigned int addr, flag_t make, page_dir_t *dir)
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
                    (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_index], 0, 0x1000);
        dir->tables_phys[table_index] = tmp | 0x7; /* present, rw, us */
        return (&dir->tables[table_index]->pages[index % 1024]);
    }

    return 0; /* did nothing */
}

/* alloc_frame()
 *  Allocate the frame in the given page entry.
 */
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

/* free_frame()
 *  Deallocate the given page entry's frame.
 */
void free_frame(page_t *page)
{
    unsigned int frame;
    if (!(frame = page->frame)) return; /* no allocated frames in page */
    clear_frame(frame);
    page->frame = 0x0;
}

void init_paging(void)
{
    unsigned int i, j, k;

    /* Set up page frames */
    nframes = 0x1000000 / 0x1000;
    frames = (unsigned int *)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));

    /* Set up a page directory */
    kernel_directory = (page_dir_t *)kmalloc_a(sizeof(page_dir_t));
    memset(kernel_directory, 0, sizeof(page_dir_t));
    kernel_directory->phys_addr = (unsigned int)kernel_directory->tables_phys;

    /* Map pages in the kernel heap */
    for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
        get_page(i, 1, kernel_directory);
    }
    
    /* Set kernel memory to readable but not writeable to user-space */
    j = 0;
    while (j < (placement_address + 0x1000)) {
        alloc_frame(get_page(j, 1, kernel_directory), 0, 0);
        j += 0x1000;
    }

    /* Allocate the pages to the kernel directory */
    for (k = KHEAP_START; k < KHEAP_START + KHEAP_INITIAL_SIZE; k += 0x1000) {
        alloc_frame(get_page(k, 1, kernel_directory), 0, 0);
    }

    /* Register a page fault handler and enable paging */
    register_interrupt_handler(14, &page_fault);
    switch_page_dir(kernel_directory);

    /* Create the kernel heap */
    kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 
            0xCFFFF000, 0, 0);

    /* Create kernel directory */
    current_directory = clone_dir(kernel_directory);
    switch_page_dir(current_directory);
}

void page_fault(registers_t regs)
{
    /* Get the faulting address from the CR2 register */
    unsigned int faulting_addr;
    __asm__ __volatile("mov %%cr2, %0" : "=r" (faulting_addr));

    /* Print some info */
    kprintf("Page fault at %p with error %d:\n", faulting_addr, regs.err_code);
    if (!(regs.err_code & 0x1)) kprintf("\tpage not present \n");
    if (regs.err_code & 0x2) kprintf("\twrite operation\n");
    if (regs.err_code & 0x4) kprintf("\t!ring 0\n");
    if (regs.err_code & 0x8) kprintf("\tcorrupted page entry\n");
    if (regs.err_code & 0x10) kprintf("\tcaused by instruction fetch\n");

    /* Panic :-( */
    panic("Page fault");
}

page_dir_t *clone_dir(page_dir_t *src)
{
    int i;
    unsigned int phys, offset;
    page_dir_t *dir = NULL;

    dir = (page_dir_t *)kmalloc_ap(sizeof(page_dir_t), &phys);
    memset(dir, 0, sizeof(page_dir_t));

    offset = (unsigned int)dir->tables_phys - (unsigned int)dir;
    dir->phys_addr = phys + offset;

    for (i = 0; i < 1024; i++) {
        if (!src->tables[i]) {
            continue;
        }

        if (kernel_directory->tables[i] == src->tables[i]) {
            /* Kernel, just use the same pointer */
            dir->tables[i] = src->tables[i];
            dir->tables_phys[i] = src->tables_phys[i];
        } else {
            /* Outside of kernel, make a copy */
            unsigned int phys;
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tables_phys[i] = phys | 0x07;
        }
    }

    return dir;
}

void move_stack(void *new_start, unsigned int size)
{
    unsigned int i, pd_addr, offset;
    unsigned int old_esp, old_ebp;
    unsigned int new_esp, new_ebp;
  
    /* Allocate enough space for new stack */
    for (i = (unsigned int)new_start; i >= ((unsigned int )new_start-size);
            i -= 0x1000) {
        alloc_frame(get_page(i, 1, current_directory), 0, 1);
    }

    /* Flush the TLB */
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (pd_addr));
    __asm__ __volatile__("mov %0, %%cr3" : : "r" (pd_addr));

    /* Get old stack pointers and work out new addresses */
    __asm__ __volatile__("mov %%esp, %0" : "=r" (old_esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r" (old_ebp));
    offset = (unsigned int)new_start - initial_esp;
    new_esp = old_esp + offset;
    new_ebp = old_ebp + offset;

    /* Copy the stack */
    memcpy((void *)new_esp, (void *)old_esp, initial_esp - old_esp);

    /* Copy each value from old stack to new */
    for (i = (unsigned int)new_start; i > (unsigned int)new_start-size; 
            i -= 4) {
        unsigned int tmp = *(unsigned int *)i;
        
        /* Remap out of range values in old stack... really inefficient >_< */
        if ((old_esp < tmp) && (tmp < initial_esp)) {
            tmp = tmp + offset;
            unsigned int *tmp2 = (unsigned int *)i;
            *tmp2 = tmp;
        }
    }

    /* Change stacks */
    __asm__ __volatile__("mov %0, %%esp" : : "r" (new_esp));
    __asm__ __volatile__("mov %0, %%ebp" : : "r" (new_ebp));
}
