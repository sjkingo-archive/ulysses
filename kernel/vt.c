#include <ulysses/trace.h>
#include <ulysses/screen.h>
#include <ulysses/vt.h>

void init_vt(void)
{
    TRACE_ONCE;
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

    active_vt = VT_LOG; /* we don't actually want to flush anything */
}

void switch_vt(unsigned short index)
{
    TRACE_ONCE;
    if (index >= NUM_VT) return;
    active_vt = index;
    flush_vt();
}

void flush_vt(void)
{
    TRACE_ONCE;
    flush_screen(virtual_terms[active_vt].data);
}

static void append_to_vt(const char c, unsigned short vt)
{
    TRACE_ONCE;
    if ((virtual_terms[vt].next + 1) >= MAX_CHARS) {
        virtual_terms[vt].next = 0;
    }
    virtual_terms[vt].data[virtual_terms[vt].next++] = c;
}

void append_char(const char c, flag_t write, flag_t all)
{
    TRACE_ONCE;
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

void remove_last_line(void)
{
    TRACE_ONCE;
    char *d = virtual_terms[active_vt].data + 
            (virtual_terms[active_vt].next - 1); /* ignore \n */
    while (*d != '\0' && *d != '\n') {
        *d = '\0';
        d--;
        virtual_terms[active_vt].next--;
    }
    flush_vt();
}
