
#include "x86.h"
#include "../../kernel/kernel.h"

#include <string.h>

extern void init_a20(void);
extern int check_a20(void);

/* panic_handler()
 *  Generic handler for interrupts that must trigger a panic() only.
 */
static void panic_handler(registers_t regs)
{
    switch (regs.int_no) {
        case 0: panic("Divide by zero error");
        case 13: panic("General protection fault");
    }
}

/* init_isrs()
 *  Register interrupt handlers for common interrupts we may get.
 */
static void init_isrs(void)
{
    register_interrupt_handler(0, &panic_handler);
    register_interrupt_handler(13, &panic_handler);
    register_interrupt_handler(33, &keyboard_handler);
}

static void set_time(void)
{
    kern.loaded_time = cmos_datetime();
    kern.current_time_offset.tv_sec = 0;
}

static void init_multiboot(void *mdb, unsigned int magic)
{
    if (magic != MULTIBOOT_LOADER_MAGIC) {
        panic("Kernel not booted by a Multiboot loader");
    }

    /* Set the multiboot info struct in the kernel's struct */
    kern.mbi = (multiboot_info_t *)mdb;

    /* Ensure that multiboot gave us a memory map, since this is required
     * when setting up paging and the heaps.
     */
    if (!MB_FLAG(kern.mbi->flags, 0)) panic("No multiboot memory info");
    if (!MB_FLAG(kern.mbi->flags, 6)) panic("No multiboot memory map");
    
    /* Copy the kernel command line from multiboot */
    kern.cmdline = (char *)kmalloc(strlen((char *)kern.mbi->cmdline) + 1);
    strcpy(kern.cmdline, (char *)kern.mbi->cmdline);
}

void startup_x86(void *mdb, unsigned int magic)
{
    /* Disable interrupts in case someone removes the cli instruction from
     * loader.s (hint: don't remove it from loader.s please :-))
     */
    __asm__ __volatile__("cli");

    /* Initialise the various low-level x86 stuff.
     *
     * The order here is important:
     *  1. Set up the video memory ready for printing
     *  2. Save things given to us by multiboot
     *  3. Load GDT
     *  4. Load IDT
     *  5. Enable the A20 address line
     *  6. Enter protected mode
     *  7. Activate memory paging
     *  8. Set kernel load time from CMOS
     *  9. Set up clock timer
     *  10. Register handlers for common interrupts
     *  11. Identify the CPU(s) attached to the system
     */
    init_screen(); /* must be before any kprintf() or panic() */
    init_multiboot(mdb, magic);
    if (!init_gdt()) panic("GDT failed initialisation\n");
    if (!init_idt()) panic("IDT failed initialisation");
    init_a20(); /* see a20.s */
    enter_pm(); /* see flush.s */
    if (!init_paging()) panic("Paging failed initialisation");
    set_time();
    if (!init_timer(TIMER_FREQ)) panic("Timer failed initialisation");
    init_isrs();
    init_cpu();

    /* And finally, enable interrupts */
    __asm__ __volatile__("sti");
}

