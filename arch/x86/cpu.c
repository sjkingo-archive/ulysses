/* arch/x86/cpu.c - CPU-specific features
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

#include <ulysses/kprintf.h>
#include <ulysses/trace.h>
#include <ulysses/util_x86.h>

extern unsigned int read_eip();

void idle_cpu(void)
{
    TRACE_ONCE;
    while(1) __asm__ __volatile__("sti ; hlt");
}

void dump_regs(void)
{
    TRACE_ONCE;
    int eip = read_eip();
    int esp = READ_ESP();
    int ebp = READ_EBP();

    int edi, esi;
    __asm__ __volatile__("mov %%edi, %0" : "=r" (edi));
    __asm__ __volatile__("mov %%esi, %0" : "=r" (esi));

    int eax, ebx, ecx, edx;
    __asm__ __volatile__("mov %%eax, %0" : "=r" (eax));
    __asm__ __volatile__("mov %%ebx, %0" : "=r" (ebx));
    __asm__ __volatile__("mov %%ecx, %0" : "=r" (ecx));
    __asm__ __volatile__("mov %%edx, %0" : "=r" (edx));

    kprintf("edi %p\tesi %p\teax %p\n", edi, esi, eax);
    kprintf("ebx %p\tecx %p\tedx %p\n", ebx, ecx, edx);
    kprintf("eip %p\tebp %p\tesp %p\n", eip, ebp, esp);
}
