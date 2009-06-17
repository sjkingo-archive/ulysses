/* arch/x86/linker.ld - GNU ld linker script for kernel
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

ENTRY (loader)
STARTUP (arch/x86/loader.o)

SECTIONS {

    .text 0x100000 : {
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(4096);
    }

    .data : {
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }

    .bss : {
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(4096);
    }

    end = .; _end = .; __end = .;
}
