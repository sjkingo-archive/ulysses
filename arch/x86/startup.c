
#include "x86.h"
#include "../../kernel/kernel.h"

static void gpf_handler(registers_t regs)
{
    kprintf("GPF: err_code: %d\n", regs.err_code);
    panic("General protection fault");
}

void startup_x86(void *mdb, unsigned int magic)
{
    init_screen(); /* must be first */

    /* Set up the multiboot stuff given to us by the boot loader */
    if (magic != MULTIBOOT_LOADER_MAGIC)
        panic("Kernel not booted by a Multiboot loader");
    kern.mbi = (multiboot_info_t *)mdb;

    /* Set up GDT, IDT and paging - NOTE the order is important! */
    if (!init_gdt()) panic("GDT failed initialisation\n");
    if (!init_idt()) panic("IDT failed initialisation");
    if (!init_paging()) panic("Paging failed initialisation");

    /* Set up a handler for GPFs so we get some nice info */
    register_interrupt_handler(13, &gpf_handler);

    /* Start a clock timer going */
    if (!init_timer(TIMER_FREQ)) panic("Timer failed initialisation");
}


