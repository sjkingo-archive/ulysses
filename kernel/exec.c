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
#include <ulysses/exec.h>
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

    struct file *f;
    struct elf_header *elf;

    f = load_file((char *)path);
    if (f == NULL) {
        errno = ENOENT;
        return -1;
    }

    elf = load_elf(f, (page_dir_t *)current_directory, TRUE);
    if (elf == NULL) {
        errno = ENOEXEC;
        return -1;
    }

    /* XXX push argc and argv on to stack */
    argv = NULL;

    /* Jump to the entry point */
    __asm__ __volatile__("mov %0, %%ecx ; jmp *%%ecx" : : "r" (elf->e_entry));

    errno = ENOEXEC;
    return -1;
}

pid_t create_init(void)
{
    TRACE_ONCE;

    struct file *f;
    struct elf_header *elf;

    /* Ensure init isn't running already */
    if (find_task("init") != NULL) {
        kprintf("create_init: init is already running\n");
        return;
    }
    
    /* Load the init file off the initrd */
    f = load_file("init");
    if (f == NULL) {
        kprintf("create_init: init not found in initrd\n");
        return;
    }
    
    /* Create a new task and load the ELF executable into memory */
    task_t *task = new_task("init");
    elf = load_elf(f, task->page_dir, TRUE);
    if (elf == NULL) {
        kprintf("create_init: init was not a valid executable\n");
        free_task(task);
        return;
    }

    /* Set the entry point and schedule the task */
    task->eip = elf->e_entry;
    add_to_queue(task);

    /* all done */
    kprintf("create_init: init process created (pid %d)\n", task->pid);
    return task->pid;
}
