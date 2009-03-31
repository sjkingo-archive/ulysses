
#include "kernel.h"

#include "vt.h"

extern flag_t shell_active; /* declared in shell.h */

void init_vt(void)
{
    unsigned short i, j;

    /* Init all the VTs to a blank state */
    for (i = 0; i < NUM_VT; i++) {
        virtual_terms[i].id = i;
        virtual_terms[i].next = 0;
        for (j = 0; j < MAX_CHARS; j++) virtual_terms[i].data[j] = '\0';

        virtual_terms[i].stdin->fd = 0;
        virtual_terms[i].stdin->buffer_pos = 0;
        virtual_terms[i].stdin->buffer_len = 1024;
    }

    switch_vt(VT_LOG);
}

void switch_vt(unsigned short index)
{
    if (index >= NUM_VT) return;
    active_vt = index;
    flush_vt();
}

void flush_vt(void)
{
    flush_screen(virtual_terms[active_vt].data);
}

static void append_to_vt(const char c, unsigned short vt)
{
    if ((virtual_terms[vt].next + 1) >= MAX_CHARS) {
        virtual_terms[vt].next = 0;
    }
    virtual_terms[vt].data[virtual_terms[vt].next++] = c;
}

void append_char(const char c, flag_t write, flag_t all)
{
    if (all) {
        unsigned short i;
        for (i = 0; i < NUM_VT; i++) {
            append_to_vt(c, i);
        }
    } else {
        append_to_vt(c, active_vt);
    }
    if (write) put_char(c); /* write through to the screen directly */
}

void append_stdin(const char c)
{
    if (virtual_terms[active_vt].stdin->buffer_pos >=
            virtual_terms[active_vt].stdin->buffer_len) {
        virtual_terms[active_vt].stdin->buffer_pos = 0;
    }

    virtual_terms[active_vt].stdin->buffer[virtual_terms[
            active_vt].stdin->buffer_pos++] = c;
    
    /* Either send the character to the shell or stdin buffer of the 
     * current VT.
     */
    if (shell_active) {
        buffer_key(c);
    } else {
        append_char(c, TRUE, FALSE);
    }
}

void up_pressed(void)
{
    if (shell_active) {
        shell_walk_history(TRUE);
    } else {
        /* XXX eat the keypress */
    }
}

void down_pressed(void)
{
    if (shell_active) {
        shell_walk_history(FALSE);
    } else {
        /* XXX eat the keypress */
    }
}
