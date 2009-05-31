/* arch/x86/keyboard.c - keyboard driver
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

#include <ulysses/isr.h>
#include <ulysses/keyboard.h>
#include <ulysses/kthread.h>
#include <ulysses/shell.h>
#include <ulysses/shutdown.h>
#include <ulysses/trace.h>
#include <ulysses/util.h>
#include <ulysses/vt.h>

static unsigned short map_unshifted[] = {
    KB_UNKNOWN, ASCII_ESC, '1', '2',                /* 0x00 - 0x03 */
    '3', '4', '5', '6',                             /* 0x04 - 0x07 */
    '7', '8', '9', '0',                             /* 0x08 - 0x0B */
    '-', '=', ASCII_BS, '\t',                       /* 0x0C - 0x0F */
    'q', 'w', 'e', 'r',                             /* 0x10 - 0x13 */
    't', 'y', 'u', 'i',                             /* 0x14 - 0x17 */
    'o', 'p', '[', ']',                             /* 0x18 - 0x1B */
    '\n', KB_LCTRL, 'a', 's',                       /* 0x1C - 0x1F */
    'd', 'f', 'g', 'h',                             /* 0x20 - 0x23 */
    'j', 'k', 'l', ';',                             /* 0x24 - 0x27 */
    '\'', '`', KB_LSHIFT, '\\',                     /* 0x28 - 0x2B */
    'z', 'x', 'c', 'v',                             /* 0x2C - 0x2F */
    'b', 'n', 'm', ',',                             /* 0x30 - 0x33 */
    '.', '/', KB_RSHIFT, KB_PRINTSCRN,              /* 0x34 - 0x37 */
    KB_LALT, ' ', KB_CAPSLOCK, KB_F1,               /* 0x38 - 0x3B */
    KB_F2, KB_F3, KB_F4, KB_F5,                     /* 0x3C - 0x3F */
    KB_F6, KB_F7, KB_F8, KB_F9,                     /* 0x40 - 0x43 */
    KB_F10, KB_NUMLOCK, KB_SCRLOCK, KP_HOME,        /* 0x44 - 0x47 */
    KP_UP, KP_PGUP, KP_MINUS, KP_LEFT,              /* 0x48 - 0x4B */
    KP_CENTER, KP_RIGHT, KP_PLUS, KP_END,           /* 0x4C - 0x4F */
    KP_DOWN, KP_PGDN, KP_INSERT, KP_DEL,            /* 0x50 - 0x53 */
    KB_SYSREQ, KB_UNKNOWN, KB_UNKNOWN, KB_UNKNOWN,  /* 0x54 - 0x57 */
};            

static unsigned short map_shifted[] = {
    KB_UNKNOWN, ASCII_ESC, '!', '@',                /* 0x00 - 0x03 */
    '#', '$', '%', '^',                             /* 0x04 - 0x07 */
    '&', '*', '(', ')',                             /* 0x08 - 0x0B */
    '_', '+', ASCII_BS, '\t',                       /* 0x0C - 0x0F */
    'Q', 'W', 'E', 'R',                             /* 0x10 - 0x13 */
    'T', 'Y', 'U', 'I',                             /* 0x14 - 0x17 */
    'O', 'P', '{', '}',                             /* 0x18 - 0x1B */
    '\n', KB_LCTRL, 'A', 'S',                       /* 0x1C - 0x1F */
    'D', 'F', 'G', 'H',                             /* 0x20 - 0x23 */
    'J', 'K', 'L', ':',                             /* 0x24 - 0x27 */
    '"', '~', KB_LSHIFT, '|',                       /* 0x28 - 0x2B */
    'Z', 'X', 'C', 'V',                             /* 0x2C - 0x2F */
    'B', 'N', 'M', '<',                             /* 0x30 - 0x33 */
    '>', '?', KB_RSHIFT, KB_PRINTSCRN,              /* 0x34 - 0x37 */
    KB_LALT, ' ', KB_CAPSLOCK, KB_F1,               /* 0x38 - 0x3B */
    KB_F2, KB_F3, KB_F4, KB_F5,                     /* 0x3C - 0x3F */
    KB_F6, KB_F7, KB_F8, KB_F9,                     /* 0x40 - 0x43 */
    KB_F10, KB_NUMLOCK, KB_SCRLOCK, KP_HOME,        /* 0x44 - 0x47 */
    KP_UP, KP_PGUP, KP_MINUS, KP_LEFT,              /* 0x48 - 0x4B */
    KP_CENTER, KP_RIGHT, KP_PLUS, KP_END,           /* 0x4C - 0x4F */
    KP_DOWN, KP_PGDN, KP_INSERT, KP_DEL,            /* 0x50 - 0x53 */
    KB_SYSREQ, KB_UNKNOWN, KB_UNKNOWN, KB_UNKNOWN,  /* 0x54 - 0x57 */
};

static unsigned short map_caps[] = {
    KB_UNKNOWN, ASCII_ESC, '1', '2',                /* 0x00 - 0x03 */
    '3', '4', '5', '6',                             /* 0x04 - 0x07 */
    '7', '8', '9', '0',                             /* 0x08 - 0x0B */
    '-', '=', ASCII_BS, '\t',                       /* 0x0C - 0x0F */
    'Q', 'W', 'E', 'R',                             /* 0x10 - 0x13 */
    'T', 'Y', 'U', 'I',                             /* 0x14 - 0x17 */
    'O', 'P', '[', ']',                             /* 0x18 - 0x1B */
    '\n', KB_LCTRL, 'A', 'S',                       /* 0x1C - 0x1F */
    'D', 'F', 'G', 'H',                             /* 0x20 - 0x23 */
    'J', 'K', 'L', ';',                             /* 0x24 - 0x27 */
    '\'', '`', KB_LSHIFT, '\\',                     /* 0x28 - 0x2B */
    'Z', 'X', 'C', 'V',                             /* 0x2C - 0x2F */
    'B', 'N', 'M', ',',                             /* 0x30 - 0x33 */
    '.', '/', KB_RSHIFT, KB_PRINTSCRN,              /* 0x34 - 0x37 */
    KB_LALT, ' ', KB_CAPSLOCK, KB_F1,               /* 0x38 - 0x3B */
    KB_F2, KB_F3, KB_F4, KB_F5,                     /* 0x3C - 0x3F */
    KB_F6, KB_F7, KB_F8, KB_F9,                     /* 0x40 - 0x43 */
    KB_F10, KB_NUMLOCK, KB_SCRLOCK, KP_HOME,        /* 0x44 - 0x47 */
    KP_UP, KP_PGUP, KP_MINUS, KP_LEFT,              /* 0x48 - 0x4B */
    KP_CENTER, KP_RIGHT, KP_PLUS, KP_END,           /* 0x4C - 0x4F */
    KP_DOWN, KP_PGDN, KP_INSERT, KP_DEL,            /* 0x50 - 0x53 */
    KB_SYSREQ, KB_UNKNOWN, KB_UNKNOWN, KB_UNKNOWN,  /* 0x54 - 0x57 */
};

/* Keyboard buffer */
static struct {
    char keys[KB_BUFFER_SIZE];
    unsigned int next_read_index;
    unsigned int next_append_index;
    flag_t shift_state;
    flag_t alt_state;
    flag_t caps_state;
} buffer;

void init_keyboard(void)
{
    TRACE_ONCE;
    buffer.next_read_index = 0;
    buffer.next_append_index = 0;
    buffer.shift_state = FALSE;
    buffer.alt_state = FALSE;
    buffer.caps_state = FALSE;
}

char next_key(void)
{
    TRACE_ONCE;
    /* Are there any more characters to be read? */
    if (buffer.next_read_index >= buffer.next_append_index) {
        return 0;
    }
    return buffer.keys[buffer.next_read_index++];
}

void keyboard_handler(registers_t regs)
{
    TRACE_ONCE;
    unsigned int scancode = inb(0x60); /* read the scancode */
    unsigned int key;

    /* Handle modifiers */
    switch (scancode) {
        case KB_MAKE_SHIFT:
            buffer.shift_state = TRUE;
            return;

        case KB_BREAK_SHIFT:
            buffer.shift_state = FALSE;
            return;

        default:
            if ((scancode & KB_IGNORE)) return;
            if (buffer.caps_state) {
                key = map_caps[scancode];
            } else {
                if (buffer.shift_state) key = map_shifted[scancode];
                else key = map_unshifted[scancode];
            }
    }

    /* Handle the key press */
    switch (key) {
        case KB_CAPSLOCK:
            if (buffer.caps_state) buffer.caps_state = FALSE;
            else buffer.caps_state = TRUE;
            return;

        case KB_LALT:
        case KB_RALT:
            if (buffer.alt_state) buffer.alt_state = FALSE;
            else buffer.alt_state = TRUE;
            return;
            
        case KB_F1:
            if (buffer.alt_state) switch_vt(0);
            return;

        case KB_F2:
            if (buffer.alt_state) switch_vt(1);
            return;

        case KB_F3:
            if (buffer.alt_state) switch_vt(2);
            return;

        case KB_F4:
            if (buffer.alt_state) switch_vt(3);
            return;
        
        case KB_F8:
            kthread_create(run_shell, "shell");
            return;

        case KB_F9:
            sanity_check();
            return;

        case KB_F10:
            panic("F10 pressed");

        case KP_UP:
            shell_walk_history(TRUE);
            return;

        case KP_DOWN:
            shell_walk_history(FALSE);
            return;

        default:
            buffer_keypress(key);
    }
}

void buffer_keypress(char key)
{
    TRACE_ONCE;
    if ((buffer.next_append_index + 1) >= KB_BUFFER_SIZE) {
        buffer.next_append_index = 0;
    }
    buffer.keys[buffer.next_append_index++] = key;
}
