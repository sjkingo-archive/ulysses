#ifndef _ULYSSES_EXCEPTIONS_H
#define _ULYSSES_EXCEPTIONS_H

#include <ulysses/isr.h>

/* Action codes that determine what should be done for each exception.
 *   ACTION_PANIC will trigger a kernel panic and CPU shutdown, and is used
 *     for unrecoverable exceptions.
 *   ACTION_KILL will cause the currently executing task to be killed and a
 *     context switch triggered. Note that if the kernel itself (pid 0) causes
 *     the exception, it will fall through to panicking.
 *   ACTION_IGNORE silently ignores the exception.
 *   ACTION_WARN outputs an informational message about the exception but
 *     otherwise continues executing.
 */
#define ACTION_PANIC 0
#define ACTION_KILL 1
#define ACTION_IGNORE 2
#define ACTION_WARN 3

struct cpu_exception {
    short num;
    char *name;
    unsigned short action;
    void (*handler)(registers_t);
};

#include <ulysses/keyboard.h>
#include <ulysses/paging.h>
#include <ulysses/syscall.h>
#include <ulysses/timer.h>

static struct cpu_exception exceptions[] = {
    /* CPU exceptions */
    { 0, "Divide by zero", ACTION_KILL, NULL, },
    { 1, "Debug", ACTION_WARN, NULL, },
    { 2, "NMI (non-maskable interrupt)", ACTION_IGNORE, NULL, },
    { 3, "Breakpoint", ACTION_WARN, NULL, },
    { 4, "Overflow", ACTION_WARN, NULL, },
    { 5, "BOUND range exceeded", ACTION_WARN, NULL, },
    { 6, "Invalid opcode", ACTION_KILL, NULL, },
    { 7, "Device not available", ACTION_KILL, NULL, },
    { 8, "Double fault", ACTION_PANIC, NULL, },
    { 9, "Coprocessor segment overrun", ACTION_PANIC, NULL, },
    { 10, "Invalid TSS", ACTION_PANIC, NULL, },
    { 11, "Segment not present", ACTION_KILL, NULL, },
    { 12, "Stack fault", ACTION_KILL, NULL, },
    { 13, "General protection fault", ACTION_KILL, NULL, },
    { 14, "Page fault", ACTION_KILL, page_fault, },
    { 15, "Unused", ACTION_IGNORE, NULL, },
    { 16, "x87 floating point error", ACTION_KILL, NULL, },
    { 17, "Alignment check", ACTION_KILL, NULL, },
    { 18, "Machine check", ACTION_PANIC, NULL, },
    { 19, "SIMD floating point", ACTION_KILL, NULL, },

    /* Custom interrupts */
    { SYSCALL, "System call", ACTION_IGNORE, syscall_handler, },

    /* IRQs mapped through the PIC(s) */
    { IRQ0, "Timer tick", ACTION_IGNORE, timer_tick, },
    { IRQ1, "Keyboard driver", ACTION_IGNORE, keyboard_handler, },

    { -1, NULL, ACTION_IGNORE, NULL } /* sentinel entry; don't remove */
};

#endif
