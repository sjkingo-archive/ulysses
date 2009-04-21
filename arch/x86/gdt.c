#include <ulysses/gdt.h>
#include <ulysses/task.h>
#include <ulysses/tss.h>
#include <ulysses/util.h>

#include <string.h>

tss_entry_t tss_entry;

extern void tss_flush(void); /* flush.s */

static void gdt_set_gate(int num, unsigned int base, unsigned int limit, 
        unsigned char access, unsigned char gran)
{
   gdt_entries[num].base_low = (base & 0xFFFF);
   gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   gdt_entries[num].base_high = (base >> 24) & 0xFF;

   gdt_entries[num].limit_low = (limit & 0xFFFF);
   gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   gdt_entries[num].granularity |= gran & 0xF0;
   gdt_entries[num].access = access;
}

static void write_tss(int num, short ss0, unsigned int esp0)
{
    unsigned int base, limit;

    base = (unsigned int)&tss_entry;
    limit = base + sizeof(tss_entry);

    gdt_set_gate(num, base, limit, 0xE9, 0x00); /* set in GDT */
    memset(&tss_entry, 0, sizeof(tss_entry));

    /* Set the kernel stack for switching */
    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;

    /* 0x08 and 0x10 are kernel code/data segments, with last two bits set. 
     * This sets the requested priv level to ring 3, allowing user mode to
     * use this TSS to jump back to ring 0 kernel mode.
     */
    tss_entry.cs = 0x0b; /* code */
    tss_entry.ss = 0x13; /* data ... */
    tss_entry.ds = 0x13;
    tss_entry.es = 0x13;
    tss_entry.fs = 0x13;
    tss_entry.gs = 0x13;
}

void init_gdt(void)
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (unsigned int)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0); /* NULL */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* kernel-mode code */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* kernel-mode data */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* user-mode code */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* user-mode data */
    write_tss(5, 0x10, 0x0);

    /* declared in flush.s */
    gdt_flush((unsigned int)&gdt_ptr);
    tss_flush();
}

void set_kernel_stack(unsigned int stack)
{
    tss_entry.esp0 = stack;
}

void switch_to_user_mode(void)
{
    CLI;
    switch_kernel_stack();

    __asm__ __volatile__("  \
            mov $0x23, %ax; \
            mov %ax, %ds;   \
            mov %ax, %es;   \
            mov %ax, %fs;   \
            mov %ax, %gs;   \
                            \
            mov %esp, %eax; \
            pushl $0x23;    \
            pushl %esp;     \
            pushf;          \
            pushl $0x1B;    \
            push $1f;       \
            iret;           \
            1:"
    );
}
