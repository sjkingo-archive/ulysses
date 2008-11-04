
#include "x86.h"
#include "../../kernel/kernel.h"

static void generic_handler(registers_t regs)
{
    switch (regs.int_no) {
        case 0: panic("Divide by zero error");
        case 13: panic("General protection fault");
    }
}

static flag_t init_pm(void)
{
    enter_pm(); /* see flush.s */
    kprintf("Switched to protected mode\n");
    return TRUE;
}

void startup_x86(void *mdb, unsigned int magic)
{
    init_screen(); /* must be first */

    /* Set up the multiboot stuff given to us by the boot loader */
    if (magic != MULTIBOOT_LOADER_MAGIC)
        panic("Kernel not booted by a Multiboot loader");
    kern.mbi = (multiboot_info_t *)mdb;

    /* Set up and load the GDT and IDT into the CPU. Then enable paging
     * and switch to protected mode. Then bring up the PIC clock timer.
     * 
     * NOTE the order is important! GDT, IDT and Paging MUST be before
     * PM!
     */
    if (!init_gdt()) panic("GDT failed initialisation\n");
    if (!init_idt()) panic("IDT failed initialisation");
    if (!init_paging()) panic("Paging failed initialisation");
    if (!init_pm()) panic("Protected mode failed initialisation");
    if (!init_timer(TIMER_FREQ)) panic("Timer failed initialisation");

    /* Register some interrupt handlers */
    register_interrupt_handler(0, &generic_handler);
    register_interrupt_handler(13, &generic_handler);

    /* And finally, enable interrupts */
    __asm__ __volatile__("sti");
}


