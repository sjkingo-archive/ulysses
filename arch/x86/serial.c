/* arch/x86/serial.c - serial port control
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

#include <ulysses/util.h>

#include <sys/types.h>

static int is_transmit_empty(unsigned int port)
{
    return inb(port + 5) & 0x20;
}

void init_serial(unsigned int port)
{
   outb(port + 1, 0x00); /* disable all interrupts */
   outb(port + 3, 0x80); /* enable DLAB (set baud rate divisor) */
   outb(port + 0, 0x03); /* 38400 baud (low byte)... */
   outb(port + 1, 0x00); /* ... and high byte */
   outb(port + 3, 0x03); /* 8 bits, no parity, one stop bit */
   outb(port + 2, 0xC7); /* enable FIFO, clear them, with 14-byte threshold */
   outb(port + 4, 0x0B); /* IRQs enabled, RTS/DSR set */
}

void write_serial(unsigned int port, const char c)
{
    /* Wait until the line is free to write to */
    while (is_transmit_empty(port) == FALSE);
    outb(port, c);
}
