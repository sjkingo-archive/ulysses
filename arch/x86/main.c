/* arch/x86/main.c - x86 entry point and low-level hardware setup
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ulysses/cmos.h>
#include <ulysses/cpuid.h>
#include <ulysses/cputest.h>
#include <ulysses/gdt.h>
#include <ulysses/idt.h>
#include <ulysses/isr.h>
#include <ulysses/kernel.h>
#include <ulysses/keyboard.h>
#include <ulysses/kheap.h>
#include <ulysses/multiboot.h>
#include <ulysses/paging.h>
#include <ulysses/serial.h>
#include <ulysses/screen.h>
#include <ulysses/shutdown.h>
#include <ulysses/syscall.h>
#include <ulysses/timer.h>
#include <ulysses/util.h>

#include <string.h>

/* These addresses are for paging and heap code to start at. This is updated
 * from the end address of kernel and multiboot modules.
 */
extern unsigned int placement_address; /* kheap.c */
extern unsigned int end; /* linker.ld */

extern void init_a20(void); /* see a20.asm */
extern void kernel_main(unsigned int); /* see kernel/kernel.c */

unsigned long long startup_time;

static void set_time(void)
{
    kern.startup_datetime = cmos_datetime();
    kern.loaded_time.tv_sec = 0;
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

    /* If any module(s) have been given, we need to move the placement address
     * to after the module(s) addresses, as not to trample over important 
     * stuff when paging is enabled.
     */
    if (kern.mbi->mods_count > 0) {
        placement_address = *(unsigned int *)(kern.mbi->mods_addr + 
                (4 * kern.mbi->mods_count));
    } else {
        placement_address = (unsigned int)&end;
    }
}

/* The kernel entry point for the x86 archticture */
void _kmain_x86(void *mdb, unsigned int magic, unsigned int initial_stack)
{
    register unsigned long long start_time;

    /* Disable interrupts in case someone removes the cli instruction from
     * loader.asm (hint: don't remove it from loader.asm please :-))
     */
    lock_kernel();

    /* Since we've disabled interrupts (and haven't even got a working timer
     * yet), we get the time from the real-time clock on the CPU. This is often
     * more accurate anyway...
     */
    start_time = rdtsc();

    /* Initialise the various low-level x86 stuff.
     *
     * The order here is important:
     *  1. Set up the video memory ready for printing
     *  2. Set up COM1 for writing log messages
     *  3. Set kernel load time from CMOS
     *  4. Save things given to us by multiboot
     *  5. Load GDT
     *  6. Load IDT
     *  7. Enable the A20 address line
     *  8. Enter protected mode
     *  9. Activate memory paging
     *  10. Identify the CPU(s) attached to the system through CPUID
     *  11. Set up clock timer
     *  12. Set up the keyboard driver
     *  13. Test for some known CPU bugs
     */
    init_screen(); /* must be before any kprintf() or panic() */
    init_serial(COM1);
    set_time();
    init_multiboot(mdb, magic);
    init_gdt();
    init_idt();
    init_a20();
    enter_pm();
    init_paging();
    get_cpuid();
    init_timer(TIMER_FREQ);
    init_keyboard();
    test_cpu_bugs();

    /* We've finished low-level setup */
    startup_time =rdtsc() - start_time;
    div64(startup_time, TSC_MAGIC);

    /* And finally, enable interrupts and load the kernel proper */
    unlock_kernel();
    kernel_main(initial_stack);

    /* If we get here, then something very bad has happened so we just give
     * up and run away like a scared mouse...
     */
    halt();
}
