/* kernel/module.c - kernel loadable module support
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
#include <ulysses/module.h>
#include <ulysses/paging.h>

#include <sys/types.h>

static struct kernel_module mods[MOD_MAX];
static unsigned short next_mod_index = 0;

void load_module(const char *name)
{
    struct file *f;
    struct elf_header *elf;
    struct kernel_module mod;
    struct module_symbol init_sym;
    struct module_symbol cleanup_sym;

    f = load_file((char *)name);
    if (f == NULL) {
        kprintf("load_module: %s not found in the initrd\n", name);
        return;
    }
    
    elf = load_elf(f, get_kernel_dir(), FALSE);
    if (elf == NULL) {
        kprintf("load_module: %s was not a valid executable\n", name);
        return;
    }

    /* Find and populate the init_module symbol */
    if ((init_sym.addr = find_symbol(f, elf, "init_module")) == -1) {
        kprintf("load_module: %s is not a valid kernel module "
                "(init_module symbol not found)\n", name);
        return;
    } else {
        init_sym.name = "init_module";
        init_sym.func = (void *)init_sym.addr;
        mod.init = init_sym;
    }

    /* Find and populate the cleanup_module symbol */
    if ((cleanup_sym.addr = find_symbol(f, elf, "cleanup_module")) == -1) {
        kprintf("load_module: %s is not a valid kernel module "
                "(cleanup_module symbol not found)\n", name);
        return;
    } else {
        cleanup_sym.name = "cleanup_module";
        cleanup_sym.func = (void *)cleanup_sym.addr;
        mod.cleanup = cleanup_sym;
    }

    /* If we got here, the module is valid and can be loaded */
    mod.name = name;
    mods[next_mod_index++] = mod;
    mod.init.func();
    kprintf("load_module: kernel module %s loaded\n", name);
}
