/* kernel/elf.c - ELF binary loader
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
#include <ulysses/paging.h>
#include <ulysses/trace.h>

#include <string.h>
#include <sys/types.h>

#define ELF_ENTRY_POINT "main"
#define ELF_ENTRY_EIP 0x080480F4

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))

/* parse_elf()
 *  Parse the given file and, provided the binary is a valid 32-bit ELF
 *  executable, return a pointer to the ELF header.
 */
static struct elf_header *parse_elf(struct file *f)
{
    struct elf_header *elf = (struct elf_header *)f->data;
    if (elf->e_ident[EI_MAG0] != 0x7F || elf->e_ident[EI_MAG1] != 'E' || 
            elf->e_ident[EI_MAG2] != 'L' || elf->e_ident[EI_MAG3] != 'F' ||
            elf->e_ident[EI_CLASS] != ELFCLASS32) {
        return NULL;
    }

    return elf;
}

/* get_symbol_string()
 *  Return the name of the symbol at index bytes offset in buf.
 */
static char *get_symbol_string(unsigned char *buf, unsigned int index)
{
    unsigned int i;
    struct elf_header *eh = (struct elf_header *)buf;

    for (i = 0; i < eh->e_shnum; i++) {
        struct elf_section_header *sh = (struct elf_section_header *)(buf + 
                eh->e_shoff + (i * eh->e_shentsize));

        /* We only care about symbol tables */
        if (sh->sh_type != SHT_SYMTAB) {
            continue;
        }
        
        /* Fetch the string table this time */
        sh = (struct elf_section_header *)(buf + eh->e_shoff + 
                (sh->sh_link * eh->e_shentsize));
        if (sh->sh_type == SHT_STRTAB) {
            if (!index) {
                return ""; /* this is isn't the same as non-existent! */
            } else {
                return (char *)(buf + sh->sh_offset + index);
            }
        }
    }

    return NULL; /* this probably won't ever happen */
}

static unsigned int get_section_offset(unsigned char* buf, int info)
{
    struct elf_header* elf = (struct elf_header *)buf;
    struct elf_section_header *header;
    header = (struct elf_section_header *)(buf + elf->e_shoff + 
            (info * elf->e_shentsize));
    return header->sh_offset;
}

/* move_sections()
 *  Map the required pages and move the ELF executable into the requested 
 *  virtual memory address. The page directory is required to know which
 *  directory (and hence process) should have the new pages mapped into.
 */
static unsigned int move_sections(struct file *f, struct elf_header *elf, page_dir_t *dir)
{
    struct elf_program_header *phdrs;
    unsigned int i;

    if (elf->e_phnum > 64) {
        kprintf("elf: %s has too many program headers, aborting move\n", 
                f->name);
        return FALSE;
    } else if (elf->e_phnum == 0) {
        kprintf("elf: %s does not have any program headers, aborting move\n", 
                f->name);
        return FALSE;
    }

    phdrs = (struct elf_program_header *)(f->data + elf->e_phoff);
    for (i = 0; i < elf->e_phnum; i++) {
        unsigned int page_start, page_end, end_padding, addr;

        /* We can only load loadable sections */
        if (phdrs[i].p_type != PT_LOAD) {
            continue;
        }
        
        page_start = phdrs[i].p_vaddr;
        page_end = (phdrs[i].p_vaddr + phdrs[i].p_memsz);
        
        /* ELF says we may need to pad the end of the binary */
        if (phdrs[i].p_memsz > phdrs[i].p_filesz) {
            end_padding = phdrs[i].p_memsz - phdrs[i].p_filesz;
        } else {
            end_padding = 0;
        }

        /* XXX flags? */

        /* Allocate pages */
        for (addr = page_start; addr < page_end; addr += 0x1000) {
            page_t *page = get_page(addr, TRUE, (page_dir_t *)dir);
            alloc_frame(page, 1, 1);
        }

        /* Copy the binary to the virtual address it wants and pad */
        memcpy((void *)phdrs[i].p_vaddr, f->data + phdrs[i].p_offset, phdrs[i].p_filesz);
        if (end_padding != 0) {
            memset((void *)page_end, 0, end_padding);
        }
    }

    return TRUE; /* ready to rock and roll */
}

#if 0
/* XXX this is buggy and doesn't appear to return the correct address */
static unsigned int find_entry_point(struct file *f, struct elf_header *elf)
{
    unsigned int i;

    for (i = 0; i < elf->e_shnum; i++) {
        struct elf_section_header *header;
        unsigned int j;
        header = (struct elf_section_header *)(f->data + elf->e_shoff + 
                (i * elf->e_shentsize));

        /* We only care about symbol tables */
        if (header->sh_type != SHT_SYMTAB) {
            continue;
        }

        for (j = 0; j < header->sh_size; j += header->sh_entsize) {
            struct elf_symbol_table *table = (struct elf_symbol_table *)(
                    f->data + header->sh_offset + j);
            char *name = get_symbol_string(f->data, table->st_name);
            
            if (strcmp(name, ELF_ENTRY_POINT) == 0) {
                return (get_section_offset(f->data, table->st_shndx) + 
                    table->st_value + (unsigned int)f->data);
            }
        }
    }

    return elf->e_entry; /* default to start of ELF */
}
#endif

static struct elf_symbol_table *fill_symbol_struct(unsigned char *buf, 
        unsigned int offset)
{
    unsigned int i;
    struct elf_header *eh = (struct elf_header *)buf;
    struct elf_symbol_table *symtab;

    for(i = 0; i < eh->e_shnum; i++) {
        struct elf_section_header *sh = (struct elf_section_header *)(buf + 
                eh->e_shoff + (i * eh->e_shentsize));
        if (sh->sh_type == 2) {
            symtab = (struct elf_symbol_table *)(buf + sh->sh_offset + 
                    (offset * sh->sh_entsize));
            return (struct elf_symbol_table *)symtab;
        }
    }

    return NULL;
}

static void perform_relocation(struct file *f, struct elf_header *elf)
{
    unsigned int i, reloc_addr, mem_addr;

    for (i = 0; i < elf->e_shnum; i++) {
        struct elf_section_header *header;
        unsigned int j;

        header = (struct elf_section_header *)(f->data + elf->e_shoff + 
                (i * elf->e_shentsize));

        /* We only care about relocatable sections */
        if (header->sh_type != SHT_REL) {
            continue;
        }

        for (j = 0; j < header->sh_size; j += header->sh_entsize) {
            struct elf_relocation *reloc = (struct elf_relocation *)(
                    f->data + header->sh_offset + j);
            struct elf_symbol_table *table = fill_symbol_struct(f->data, 
                    ELF32_R_SYM(reloc->r_info));

            /* Resolve this reference */
            char *sym_name = get_symbol_string(f->data, table->st_name);
            if (strcmp(sym_name, "") == 0) {
                continue;
            }

            symbol_t *s = get_trace_symbol(sym_name);
            if (s == NULL) {
                kprintf("perform_relocation: unresolved relative "
                        "symbol %s\n", sym_name);
                continue;
            }

            kprintf("Resolving %s to %p\n", sym_name, s->addr);

            /* Perform the actual relocation */
            switch (ELF32_R_TYPE(reloc->r_info)) {
                case R_ABSOL:
                    mem_addr = (unsigned int)f->data + reloc->r_offset;
                    mem_addr += get_section_offset(f->data, header->sh_info);

                    if (table->st_shndx == 0) {
                        reloc_addr = (unsigned int)s->addr;
                    } else {
                        reloc_addr = (unsigned int)f->data + table->st_value +
                            *(int *)mem_addr;
                        reloc_addr += get_section_offset(f->data,
                                table->st_shndx);
                    }
                    *(unsigned int *)mem_addr = reloc_addr;
                    kprintf("relocated %s to %p\n", sym_name, (void *)reloc_addr);
                    break;

                case R_REL:
                    mem_addr = (unsigned int)f->data + reloc->r_offset;
                    mem_addr += get_section_offset(f->data, header->sh_info);

                    if (table->st_shndx == 0) {
                        reloc_addr = (unsigned int)s->addr;
                    } else {
                        reloc_addr = (unsigned int)f->data + table->st_value;
                        reloc_addr += get_section_offset(f->data, 
                                table->st_shndx);
                    }

                    reloc_addr = mem_addr - reloc_addr + 4;
                    *(unsigned int *)mem_addr = -reloc_addr;
                    kprintf("relocated %s to %p\n", sym_name, (void *)reloc_addr);
                    break;

                default:
                    kprintf("Unknown relocation type %X\n", 
                            ELF32_R_SYM(reloc->r_info));
            }
        }
    }
}

/* print_elf()
 *  Print a one line representation of the ELF executable, similar to Linux's
 *  file(1).
 */
static void print_elf(char *name, struct elf_header *elf)
{
    kprintf("%s:", name);

    /* File's class */
    switch (elf->e_ident[EI_CLASS]) {
        case ELFCLASS32:
            kprintf(" 32-bit");
            break;
        case ELFCLASS64:
            kprintf(" 64-bit");
            break;
        default:
            kprintf(" invalid class %d", elf->e_ident[EI_CLASS]);
    }

    /* Data encoding */
    switch (elf->e_ident[EI_DATA]) {
        case ELFDATA2LSB:
            kprintf(" LSB");
            break;
        case ELFDATA2MSB:
            kprintf(" MSB");
            break;
        default:
            kprintf(" invalid data encoding,");
    }

    /* Find file type */
    switch (elf->e_type) {
        case ET_NONE:
            kprintf(" no file type,");
            break;
        case ET_REL:
            kprintf(" relocatable,");
            break;
        case ET_EXEC:
            kprintf(" executable,");
            break;
        case ET_DYN:
            kprintf(" shared object,");
            break;
        case ET_CORE:
            kprintf(" core dump,");
            break;
        default:
            kprintf(" unknown type %d,", elf->e_type);
    }
    
    switch (elf->e_machine) {
        case EM_NONE:
            kprintf(" no machine,");
            break;
        case EM_386:
            kprintf(" Intel 80386,");
            break;
        default:
            kprintf(" unknown machine.");
    }

    kprintf(" entry point at %p,", (void *)elf->e_entry);
    kprintf(" %d segments\n", elf->e_phnum);
}

static void add_kernel_symbols(struct file *f, struct elf_header *elf)
{
    unsigned int i;

    for (i = 0; i < elf->e_shnum; i++) {
        struct elf_section_header *header;
        unsigned int j;
        header = (struct elf_section_header *)(f->data + elf->e_shoff + 
                (i * elf->e_shentsize));

        /* We only care about symbol tables */
        if (header->sh_type != SHT_SYMTAB) {
            continue;
        }

        for (j = 0; j < header->sh_size; j += header->sh_entsize) {
            struct elf_symbol_table *table = (struct elf_symbol_table *)(
                     f->data + header->sh_offset + j);
            char *str = get_symbol_string(f->data, table->st_name);
            if (str != NULL && strcmp(str, "") != 0) {
                add_trace_symbol(str, (void *)table->st_value);
            }
        }
        kprintf("\n");
    }
}

/* dump_symbols()
 *  Print all of the symbols and their addresses in the given ELF executable.
 */
void dump_symbols(struct file *f, struct elf_header *elf)
{
    unsigned int i;

    for (i = 0; i < elf->e_shnum; i++) {
        struct elf_section_header *header;
        unsigned int j;
        header = (struct elf_section_header *)(f->data + elf->e_shoff + 
                (i * elf->e_shentsize));

        /* We only care about symbol tables */
        if (header->sh_type != SHT_SYMTAB) {
            continue;
        }

        for (j = 0; j < header->sh_size; j += header->sh_entsize) {
            struct elf_symbol_table *table = (struct elf_symbol_table *)(
                     f->data + header->sh_offset + j);
            char *str = get_symbol_string(f->data, table->st_name);
            if (str != NULL && strcmp(str, "") != 0) {
                kprintf("%p %s\n", (void *)table->st_value, str);
            }
        }
        kprintf("\n");
    }
}

int find_symbol(struct file *f, struct elf_header *elf, const char *name)
{
    unsigned int i;

    for (i = 0; i < elf->e_shnum; i++) {
        struct elf_section_header *header;
        unsigned int j;
        header = (struct elf_section_header *)(f->data + elf->e_shoff + 
                (i * elf->e_shentsize));

        /* We only care about symbol tables */
        if (header->sh_type != SHT_SYMTAB) {
            continue;
        }

        for (j = 0; j < header->sh_size; j += header->sh_entsize) {
            struct elf_symbol_table *table = (struct elf_symbol_table *)(
                    f->data + header->sh_offset + j);
            char *str = get_symbol_string(f->data, table->st_name);
            
            if (strcmp(str, name) == 0) {
                return (get_section_offset(f->data, table->st_shndx) + 
                    table->st_value + (unsigned int)f->data);
            }
        }
    }

    return -1;
}

struct elf_header *load_elf(struct file *f, page_dir_t *dir, flag_t move)
{
    struct elf_header *elf = parse_elf(f);
    if (elf == NULL) {
        kprintf("elf: %s is not a valid 32-bit ELF binary\n", f->name);
        return NULL;
    }

    print_elf(f->name, elf);
    if (move && !move_sections(f, elf, dir)) {
        return NULL;
    }

    perform_relocation(f, elf);

    elf->e_entry = (unsigned int)ELF_ENTRY_POINT;
    return elf;
}

void load_kernel_symbols(void)
{
    struct file *f;
    f = load_file("kernel");
    if (f == NULL) {
        kprintf("load_kernel: kernel not found in initrd\n");
        return;
    }

    struct elf_header *elf = parse_elf(f);
    if (elf == NULL) {
        kprintf("load_kernel: %s is not a valid 32-bit ELF binary\n", f->name);
        return;
    }

    add_kernel_symbols(f, elf);
}
