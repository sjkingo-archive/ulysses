/* kernel/fs/mount.c - common file system mount routines
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

#include <ulysses/fs.h>
#include <ulysses/kprintf.h>

#include <string.h>
#include <sys/types.h>

flag_t mount_fs(int disk, const char *fs_type)
{
    void *fs = NULL;
    if (strcmp(fs_type, "ext2") == 0) {
        fs = (void *)mount_ext2(disk);
    } else {
        kprintf("Unknown file system type `%s`\n", fs_type);
    }

    if (fs == NULL) {
        kprintf("Failed to mount %s file system from %d\n", fs_type, disk);
        return FALSE;
    }

    return TRUE;
}
