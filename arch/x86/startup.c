#include <ulysses/cmos.h>
#include <ulysses/cpu.h>
#include <ulysses/gdt.h>
#include <ulysses/idt.h>
#include <ulysses/isr.h>
#include <ulysses/kernel.h>
#include <ulysses/keyboard.h>
#include <ulysses/kheap.h>
#include <ulysses/multiboot.h>
#include <ulysses/paging.h>
#include <ulysses/screen.h>
#include <ulysses/shutdown.h>
#include <ulysses/timer.h>
#include <ulysses/util.h>

#include <string.h>

extern void init_a20(void); /* see a20.s */

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

/* register_fatal_isrs()
 *  Register interrupt handlers for fatal interrupts.
 */
static void register_fatal_isrs(void)
{
    register_interrupt_handler(0, &panic_handler);
    register_interrupt_handler(13, &panic_handler);
}

/* register_common_isrs()
 *  Register interrupt handlers for non-fatal common interrupts.
 */
static void register_common_isrs(void)
{
    register_interrupt_handler(IRQ1, &keyboard_handler);
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
    CLI;

    /* Initialise the various low-level x86 stuff.
     *
     * The order here is important:
     *  1. Set up the video memory ready for printing
     *  2. Save things given to us by multiboot
     *  3. Load GDT
     *  4. Load IDT
     *  5. Register handlers for fatal interrupts
     *  6. Enable the A20 address line
     *  7. Enter protected mode
     *  8. Activate memory paging
     *  9. Set kernel load time from CMOS
     *  10. Set up clock timer
     *  11. Register handlers for common interrupts
     *  12. Identify the CPU(s) attached to the system
     */
    init_screen(); /* must be before any kprintf() or panic() */
    init_multiboot(mdb, magic);
    init_gdt();
    init_idt();
    register_fatal_isrs();
    init_a20(); /* see flush.s */
    enter_pm(); /* see flush.s */
    init_paging();
    set_time();
    init_timer(TIMER_FREQ);
    init_cpu();
    register_common_isrs();

    /* And finally, enable interrupts */
    STI;

