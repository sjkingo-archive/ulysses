/* arch/x86/smbios.c - SMBIOS access
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

#include <ulysses/smbios.h>
#include <ulysses/shutdown.h>

#include <string.h>
#include <sys/types.h>

/* The entry table structure */
static struct smbios_entry *entry = NULL;

/* locate_smbios()
 *  Find the SMBIOS entry table in memory, and return its start address.
 */
static void *locate_smbios(void)
{
    char *mem = (char *)0xF0000;
    while ((unsigned int)mem < 0x100000) {
        if (mem[0] == '_' && mem[1] == 'S' && 
                mem[2] == 'M' && mem[3] == '_') {
            return (void *)mem;
        }
        mem++;
    }
    return NULL;
}

void read_smbios(void)
{
    void *s = locate_smbios();
    if (s == NULL) {
        panic("SMBIOS structure not found in memory");
    }

    /* Make sure the structure is valid */
    entry = (struct smbios_entry *)s;
    if (entry->sm_str[0] != '_' || entry->sm_str[1] != 'S' ||
                entry->sm_str[2] != 'M' || entry->sm_str[3] != '_') {
        panic("SMBIOS structure found, but identification failed");
    }
}
