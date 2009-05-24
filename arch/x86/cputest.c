/* arch/x86/cputest.c - CPU-specific tests
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

#include <sys/types.h>

void test_f00f(void)
{
    __asm__ __volatile__("mov %0, %%eax; lock cmpxchg8b (%%eax)" : : "r" (0));
}

flag_t test_fdiv(void)
{
    if ((float)4195835*3145727/3145727 != 4195835) {
        return TRUE;
    }
    return FALSE;
}
