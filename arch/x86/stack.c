/* arch/x86/stack.c - stack-related functions
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

static void unwind_stack(void **ebp, void *end)
{
    void *eip = (void *)read_eip();
    unsigned int i = 0;

    while (eip <= end && ebp) {
        symbol_t *sym = get_closest_symbol(eip);
        if (sym == NULL) {
            kprintf(" #%d %p in ???\n", i, eip);
            kprintf("(stack appears to be trashed -- aborting trace)\n");
            break;
        } else {
            kprintf(" #%d %p in %s ()\n", i++, eip, sym->name);
        }

        /* unwind */
        eip = ebp[1];
        ebp = (void **)ebp[0];
    }
}

void stack_trace(void)
{
    void *end;

    symbol_t *end_sym = get_trace_symbol("__end");
    if (end_sym == NULL) {
        end = (void *)0x200000; /* provide some sentinel protection */
    } else {
        end = end_sym->addr;
    }

    kprintf("Call trace (from top of stack):\n");
    unwind_stack((void **)READ_EBP(), end); /* kernel */
}
