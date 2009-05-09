#include <ulysses/cmos.h>
#include <ulysses/cpuid.h>
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
#include <ulysses/syscall.h>
#include <ulysses/timer.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <string.h>

extern void init_a20(void); /* see a20.s */

static void set_time(void)
{
    TRACE_ONCE;
    kern.startup_datetime = cmos_datetime();
    kern.loaded_time.tv_sec = 0;
    kern.current_time_offset.tv_sec = 0;
}

static void init_multiboot(void *mdb, unsigned int magic)
{
    TRACE_ONCE;
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
    TRACE_ONCE;

    /* Disable interrupts in case someone removes the cli instruction from
     * loader.s (hint: don't remove it from loader.s please :-))
     */
    CLI;

    /* Initialise the various low-level x86 stuff.
     *
     * The order here is important:
     *  1. Set up the video memory ready for printing
     *  2. Set kernel load time from CMOS
     *  3. Save things given to us by multiboot
     *  4. Load GDT
     *  5. Load IDT
     *  6. Enable the A20 address line
     *  7. Enter protected mode
     *  8. Activate memory paging
     *  9. Set up clock timer
     *  10. Identify the CPU(s) attached to the system
     *  11. Set up the keyboard driver
     */
    init_screen(); /* must be before any kprintf() or panic() */
    set_time();
    init_multiboot(mdb, magic);
    init_gdt();
    init_idt();
    init_a20(); /* see flush.s */
    enter_pm(); /* see flush.s */
    init_paging();
    init_timer(TIMER_FREQ);
    get_cpuid();
    init_keyboard();

    /* And finally, enable interrupts */
    STI;
}
