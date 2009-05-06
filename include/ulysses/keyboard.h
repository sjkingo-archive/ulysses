#ifndef _ULYSSES_KEYBOARD_H
#define _ULYSSES_KEYBOARD_H

/* This is the keyboard driver. A callback into this module is placed at 
 * startup to intercept IRQ1. We then read from the PIC that sent the IRQ
 * and get back a scancode: a hexidecimal code that signifies the key pressed.
 * 
 * The scancode is converted into a key by use of the map_ tables. Once 
 * converted, the key is appended to the ring buffer. This can be read by
 * the read_next_key() function and is mapped to the stdin file stream in
 * libc.
 */

#include <ulysses/isr.h> /* for registers_t */

#define KB_BUFFER_SIZE 255

#define KB_MAKE_SHIFT 0x2A
#define KB_BREAK_SHIFT 0xAA
#define KB_IGNORE 0x80

/* Flags */
#define KB_SPECIAL_FLAG 0x0100
#define KEYPAD_FLAG 0x0200
#define KB_SHIFT_FLAG 0x1000
#define KB_ALT_FLAG 0x2000
#define KB_CTRL_FLAG 0x4000
#define KB_RELEASE_FLAG 0x8000

/* Special keys */
#define SPECIAL(n) (KB_SPECIAL_FLAG | (n))
#define KB_UNKNOWN SPECIAL(0)
#define KB_F1 SPECIAL(1)
#define KB_F2 SPECIAL(2)
#define KB_F3 SPECIAL(3)
#define KB_F4 SPECIAL(4)
#define KB_F5 SPECIAL(5)
#define KB_F6 SPECIAL(6)
#define KB_F7 SPECIAL(7)
#define KB_F8 SPECIAL(8)
#define KB_F9 SPECIAL(9)
#define KB_F10 SPECIAL(10)
#define KB_F11 SPECIAL(11)
#define KB_F12 SPECIAL(12)
#define KB_LCTRL SPECIAL(13)
#define KB_RCTRL SPECIAL(14)
#define KB_LSHIFT SPECIAL(15)
#define KB_RSHIFT SPECIAL(16)
#define KB_LALT SPECIAL(17)
#define KB_RALT SPECIAL(18)
#define KB_PRINTSCRN SPECIAL(19)
#define KB_CAPSLOCK SPECIAL(20)
#define KB_NUMLOCK SPECIAL(21)
#define KB_SCRLOCK SPECIAL(22)
#define KB_SYSREQ SPECIAL(23)

/* Keypad keys */
#define KP_START 128
#define KEYPAD(n) (KEYPAD_FLAG | KB_SPECIAL_FLAG | (n+KP_START))
#define KP_HOME KEYPAD(0)
#define KP_UP KEYPAD(1)
#define KP_PGUP KEYPAD(2)
#define KP_MINUS KEYPAD(3)
#define KP_LEFT KEYPAD(4)
#define KP_CENTER KEYPAD(5)
#define KP_RIGHT KEYPAD(6)
#define KP_PLUS KEYPAD(7)
#define KP_END KEYPAD(8)
#define KP_DOWN KEYPAD(9)
#define KP_PGDN KEYPAD(10)
#define KP_INSERT KEYPAD(11)
#define KP_DEL KEYPAD(12)

/* ASCII codes */
#define ASCII_ESC 0x1B
#define ASCII_BS 0x08

/* init_keyboard()
 *  Set up the keyboard driver and register a handler for IRQ1.
 */
void init_keyboard(void);

/* next_key()
 *  Returns the next character in the keyboard buffer, or 0 if there are no
 *  more characters waiting to be read.
 */
char next_key(void);

/* keyboard_handler()
 *   Handler callback for IRQ1. Reads the scancode from the PIC and maps
 *   it to a key.
 */
void keyboard_handler(registers_t regs);

#endif
