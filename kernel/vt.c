
#include "kernel.h"
#include "vt.h"

flag_t init_vt(void)
{
    unsigned short i, j;

    /* Init all the VTs to a blank state */
    for (i = 0; i < NUM_VT; i++) {
        virtual_terms[i].id = i;
        virtual_terms[i].next = 0;
        for (j = 0; j < MAX_CHARS; j++) virtual_terms[i].data[j] = '\0';
    }

    /* Switch to the first VT */
    if (!switch_vt(0)) return FALSE;
    kprintf("Switched to VT 0\n");

    return TRUE; /* ensure a sane return for _kmain() */
}

flag_t switch_vt(unsigned short index)
{
    if (index >= NUM_VT) return FALSE;
    active_vt = index;
    flush_vt();
    return TRUE;
}

void flush_vt(void)
{
    flush_screen(virtual_terms[active_vt].data);
}

void append_char(const char c, flag_t write)
{
    virtual_terms[active_vt].data[virtual_terms[active_vt].next++] = c;
    if (write) put_char(c); /* write through to the screen directly */
}

