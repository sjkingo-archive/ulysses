
#ifndef _x86_H
#define _x86_H

/* Put x86-specific stuff here. */

#include "gdt.h" /* global descriptor table */
#include "idt.h" /* IDT */
#include "isr.h" /* interrupts */
#include "kheap.h"
#include "multiboot.h"
#include "paging.h"
#include "timer.h"
#include "screen.h"
#include "util.h"

/* kernel includes */
#include "../config.h"
#include "../kernel/kprintf.h"
#include "../kernel/shutdown.h"

/* startup_x86()
 *  Perform x86-specific startup.
 */
void startup_x86(void *mdb, unsigned int magic);

#endif

