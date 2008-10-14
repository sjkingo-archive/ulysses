
#ifndef _x86_H
#define _x86_H

/* Put x86-specific stuff here. */

#include "gdt.h" /* global descriptor table */
#include "isr/idt.h" /* IDT */
#include "isr/isr.h" /* interrupts */
#include "multiboot.h"
#include "paging.h"
#include "timer.h"
#include "screen.h"
#include "util.h"

/* startup_x86()
 *  Perform x86-specific startup.
 */
void startup_x86(void *mdb, unsigned int magic);

#endif

