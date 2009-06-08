/* arch/x86/isr.c - interrupt vector handling
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

#include "../../config.h"
#include <ulysses/exceptions.h>
#include <ulysses/isr.h>
#include <ulysses/kernel.h>
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>

#include <sys/types.h>

static inline void handle_exception(struct cpu_exception exc)
{
    TRACE_ONCE;

    /* Execute the action */
    switch (exc.action) {
        case ACTION_PANIC:
            panic(exc.name);
            break;

        case ACTION_KILL:
            if (do_getpid() == 0) {
                panic("Kernel task caused a CPU exception");
            }
            kprintf("%s in pid %d: killing it.\n", exc.name, do_getpid());
            task_exit();
            break; /* would never get here anyway */

        case ACTION_WARN:
            kprintf("%s in pid %d\n", exc.name, do_getpid());
        case ACTION_IGNORE:
            break;
    }
}

static inline flag_t lookup_exception(registers_t *regs)
{
    TRACE_ONCE;

    struct cpu_exception exc;
    int i = 0;

    while (exceptions[i].name != NULL) {
        if (exceptions[i].num == (short)regs->int_no) {
            exc = exceptions[i];
            if (exc.handler != NULL) {
                exc.handler(regs);
            }
            handle_exception(exc);
            return TRUE;
        }
        i++;
    }

    return FALSE;
}

void isr_handler(registers_t regs)
{
    TRACE_ONCE;

    if (kern.flags.debug_interrupt) {
        symbol_t *sym;
        void *addr = __builtin_return_address(1);

        kprintf("\nInterrupt %d at %p, inside %p ", regs.int_no, regs.eip, 
                addr);

        /* Try to translate the last func's eip into a symbol */
        sym = lookup_symbol(addr);
        if (sym == NULL) kprintf("in ??? ()\n");
        else kprintf("in %s ()\n", sym->name);

        kprintf("err_code %d; dumping registers:\n", regs.err_code);
        kprintf("edi %p\tesi %p\tebp %p\tesp %p\n"
                "ebx %p\tedx %p\tecx %p\teax %p\n", regs.edi, regs.esi, 
                regs.ebp, regs.esp, regs.ebx, regs.edx, regs.ecx, regs.eax);
    }

    /* Lookup and handle the exception */
    if (!lookup_exception(&regs)) {
        panic("Unknown exception");
    }
}

void irq_handler(registers_t regs)
{
    TRACE_ONCE;

    /* Write an end of interrupt signal to the PICs. This needs to be done
     * *before* executing any exception handler, since the IRQ line will not
     * get reactivated until the EOI is sent.
     */
    if (regs.int_no >= IRQ0) {
        if (regs.int_no >= IRQ8) outb(0xA0, 0x20); /* slave */
        outb(0x20, 0x20); /* master */
    }

    /* Lookup and execute the handler for this IRQ. Normally we would check
     * the return value of this in case a handler could not be found, but
     * since we don't care about most IRQs: ignore it.
     */
    lookup_exception(&regs);
}
