/* kernel/kprintf.c - printf() compatible implementation for the kernel
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

#include <ulysses/kprintf.h>
#include <ulysses/serial.h>
#include <ulysses/screen.h>
#include <ulysses/vt.h>

#include <stdarg.h>

/* kputc()
 *  Append a raw character. All other kput..() call this.
 */
static void kputc(const char c)
{
    append_char(c, TRUE);
    write_serial(COM1, c);
}

/* kputs()
 *  Print a NULL-terminated string character-by-character.
 */
static void kputs(const char *str)
{
    const char *ptr; /* since we may want to change what str is pointing to */
    if (str == NULL) {
        ptr = "(null)";
    } else {
        ptr = str;
    }

    while (*ptr != '\0') {
        kputc(*ptr);
        ptr++;
    }
}

int kprintf(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	int bytes;

	va_start(args, fmt);
	bytes = vsprintf(buf, fmt, args);
	va_end(args);

    kputs(buf);
	return bytes;
}

void do_kdebug(const char *file, int line, const char *func, 
        const char *fmt, ...)
{
    char buf[1024];
    va_list args;

    va_start(args, fmt);
    bytes = vsprintf(buf, fmt, args);
    va_end(args);

    kprintf("DEBUG at %s:%d in %s(): %s\n", file, line, func, buf);
}
