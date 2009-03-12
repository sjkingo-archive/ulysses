
#include "keyboard.h"
#include "util.h" /* for inb() */
#include "../../kernel/kernel.h"

unsigned short map_unshifted[] = {
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

unsigned short map_shifted[] = {
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

unsigned short map_caps[] = {
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

/* shift_state
 * alt_state
 * caps_state
 *   Remembers the current state of the shift and alt keys.
 */
flag_t shift_state = FALSE;
flag_t alt_state = FALSE;
flag_t caps_state = FALSE;

void keyboard_handler(registers_t regs)
{
    unsigned int scancode = inb(0x60); /* read the scancode */
    unsigned int key;

    /* Handle modifiers */
    switch (scancode) {
        case KB_MAKE_SHIFT:
            shift_state = TRUE;
            return;

        case KB_BREAK_SHIFT:
            shift_state = FALSE;
            return;

        default:
            if ((scancode & KB_IGNORE)) return;
            if (caps_state) {
                key = map_caps[scancode];
            } else {
                if (shift_state) key = map_shifted[scancode];
                else key = map_unshifted[scancode];
            }
    }

    /* Handle the key press */
    switch (key) {
        case KB_CAPSLOCK:
            if (caps_state) caps_state = FALSE;
            else caps_state = TRUE;
            return;

        case KB_LALT:
        case KB_RALT:
            if (alt_state) alt_state = FALSE;
            else alt_state = TRUE;
            return;
            
        case KB_F1:
            if (alt_state) switch_vt(0);
            return;

        case KB_F2:
            if (alt_state) switch_vt(1);
            return;

        case KB_F3:
            if (alt_state) switch_vt(2);
            return;

        case KB_F4:
            if (alt_state) switch_vt(3);
            return;

        case KB_F9:
            sanity_check();
            return;

        case KB_F10:
            panic("F10 pressed");

        case KP_UP:
            up_pressed();
            return;

        case KP_DOWN:
            down_pressed();
            return;

        default:
            append_stdin(key);
    }
}

