/* kernel/exec.c - execute commands from initrd
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

#include <ulysses/elf.h>
#include <ulysses/kprintf.h>
#include <ulysses/initrd.h>
#include <ulysses/task.h>
#include <ulysses/trace.h>
#include <ulysses/sched.h>

#include <errno.h>
#include <sys/types.h>

extern volatile page_dir_t *current_directory;

int do_execv(const char *path, char *const argv[])
{
    TRACE_ONCE;

    int entry;
    struct file *f;

    f = load_file((char *)path);
    if (f == NULL) {
        errno = ENOENT;
        return -1;
    }

    entry = load_elf(f, (page_dir_t *)current_directory);
    if (entry == -1) {
        errno = ENOEXEC;
        return -1;
    }

    /* XXX push argc and argv on to stack */
    argv = NULL;

    /* Jump to the entry point */
    __asm__ __volatile__("mov %0, %%ecx ; jmp *%%ecx" : : "r" (entry));

    errno = ENOEXEC;
    return -1;
}
