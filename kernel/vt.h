
#ifndef _VT_H
#define _VT_H

/* Virtual terminals are abstractions of a screen, buffered in memory. They
 * are hardware-independant, and rely on the underlying arch-specific screen
 * handling to actually write something to an output device.
 */

#include <sys/types.h>

#define NUM_VT 4
#define VT_LOG (NUM_VT - 1)

#define MAX_CHARS 10000 /* XXX */
typedef struct _vt {
    unsigned short id;
    unsigned short next; /* next index in buffer to write a char to */
    char data[MAX_CHARS];
} vt_t;

vt_t virtual_terms[NUM_VT]; /* pointers to all the VTs */
unsigned short active_vt; /* the active VT that gets output */

/* init_vt()
 *  Initialises all of the virtual terminals to a blank state. This will also
 *  copy any output already written to screen into the first VT and flush it,
 *  so it is not lost.
 */
flag_t init_vt(void);

/* switch_vt()
 *  Change the active virtual terminal to the one at index. This will flush
 *  the VT to the screen.
 */
flag_t switch_vt(unsigned short index);

/* flush_vt()
 *  Flushes the active VT to screen using arch-specific methods.
 */
void flush_vt(void);

/* append_char()
 *  Appends the given character to the current VT's buffer and writes it to
 *  screen if write is true.
 */
void append_char(const char c, flag_t write);

#endif

