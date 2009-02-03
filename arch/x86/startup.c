
#include "x86.h"
#include "../../kernel/kernel.h"

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

void startup_x86(void *mdb, unsigned int magic)
{
    /* Disable interrupts in case someone removes the cli instruction from
     * loader.s (hint: don't remove it from loader.s please :-))
     */
    __asm__ __volatile__("cli");

    init_screen(); /* must be before any kprintf() or panic() */

    /* Set up the multiboot stuff given to us by the boot loader */
    if (magic != MULTIBOOT_LOADER_MAGIC)
        panic("Kernel not booted by a Multiboot loader");
    kern.mbi = (multiboot_info_t *)mdb;

    /* Initialise the various low-level x86 stuff.
     *
     * The order here is important:
     *  1. Load GDT
     *  2. Enter protected mode
     *  3. Load IDT
     *  4. Activate memory paging
     *  5. Set up clock timer
     *  6. Register handlers for common interrupts
     *  7. Identify the CPU(s) attached to the system
     */
    if (!init_gdt()) panic("GDT failed initialisation\n");
    enter_pm(); /* see flush.s */
    if (!init_idt()) panic("IDT failed initialisation");
    if (!init_paging()) panic("Paging failed initialisation");
    if (!init_timer(TIMER_FREQ)) panic("Timer failed initialisation");
    init_isrs();
    init_cpu();

    /* And finally, enable interrupts */
    __asm__ __volatile__("sti");
}

