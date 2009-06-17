/* arch/x86/multiboot.c - Multiboot-specific features
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

#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/multiboot.h>

void print_memory_map(void)
{
    memory_map_t *mmap;

    if (!MB_FLAG(kern.mbi->flags, 0) || !MB_FLAG(kern.mbi->flags, 0)) {
        kprintf("No multiboot-provided physical memory map\n");
        return;
    }

    kprintf("Detected %u KB of lower and %u KB of upper memory\n", 
            kern.mbi->mem_lower, kern.mbi->mem_upper);
    kprintf("Multiboot-provided physical memory map:\n");

    for (mmap = (memory_map_t *)kern.mbi->mmap_addr; (unsigned long)mmap < 
            (kern.mbi->mmap_addr + kern.mbi->mmap_length);
            mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + 
            sizeof(mmap->size))) {
        kprintf("  %p - %p ", mmap->base_addr_low, (mmap->base_addr_low +
                    mmap->length_low));
        switch (mmap->type) {
            case MB_RAM:
                kprintf("(usable)\n");
                break;
            case MB_RESERVED:
                kprintf("(reserved)\n");
                break;
            case MB_ACPI:
            case MB_NVS:
                kprintf("(ACPI)\n");
                break;
            case MB_UNUSABLE:
                kprintf("(unusable)\n");
                break;
            default:
                kprintf("(unknown type %d)\n", mmap->type);
        }
    }
}
