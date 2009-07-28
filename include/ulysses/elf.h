#ifndef _ULYSSES_ELF_H
#define _ULYSSES_ELF_H

#include <ulysses/initrd.h>
#include <ulysses/paging.h>

#include <sys/types.h>

struct elf_header {
    unsigned char e_ident[16];
    unsigned short e_type;
    unsigned short e_machine;
    unsigned int e_version;
    unsigned int e_entry;
    unsigned int e_phoff;
    unsigned int e_shoff;
    unsigned int e_flags;
    unsigned short e_ehsize;
    unsigned short e_phentsize;
    unsigned short e_phnum;
    unsigned short e_shentsize;
    unsigned short e_shnum;
    unsigned short e_shstrndx;
};

struct elf_program_header {
    unsigned int p_type;
    unsigned int p_offset;
    unsigned int p_vaddr;
    unsigned int p_paddr;
    unsigned int p_filesz;
    unsigned int p_memsz;
    unsigned int p_flags;
    unsigned int p_align;
};

struct elf_section_header {
    unsigned int sh_name;
    unsigned int sh_type;
    unsigned int sh_flags;
    unsigned int sh_addr;
    unsigned int sh_offset;
    unsigned int sh_size;
    unsigned int sh_link;
    unsigned int sh_info;
    unsigned int sh_addralign;
    unsigned int sh_entsize;
};

struct elf_symbol_table {
    unsigned int st_name;
    unsigned int st_value;
    unsigned int st_size;
    unsigned char st_info;
    unsigned char st_other;
    unsigned short st_shndx;
};

struct elf_relocation {
    unsigned int r_offset;
    unsigned int r_info;
};

enum e_ident {
    EI_MAG0,
    EI_MAG1,
    EI_MAG2,
    EI_MAG3,
    EI_CLASS,
    EI_DATA,
    EI_VERSION,
    EI_PAD,
    EI_NIDENT=16,
};

enum e_class {
    ELFCLASSNONE,
    ELFCLASS32,
    ELFCLASS64,
};

enum e_data {
    ELFDATANONE,
    ELFDATA2LSB,
    ELFDATA2MSB,
};

enum e_type { 
    ET_NONE,
    ET_REL, 
    ET_EXEC,
    ET_DYN,
    ET_CORE
};

enum e_machine {
    EM_NONE,
    EM_M32,
    EM_SPARC,
    EM_386,
    EM_68K,
    EM_88K,
    EM_860=7,
    EM_MIPS=8,
    EM_MIPS_RS4_BE=10,
};

enum e_seg {
    PT_NULL,
    PT_LOAD,
    PT_DYNAMIC,
    PT_INTERP,
    PT_NOTE,
    PT_SHLIP,
    PT_PHDR,
};

enum sh_type {
    SHT_NULL,
    SHT_PROGBITS,
    SHT_SYMTAB,
    SHT_STRTAB,
    SHT_RELA,
    SHT_HASH,
    SHT_DYNAMIC,
    SHT_NOTE,
    SHT_NOTBITS,
    SHT_REL,
    SHT_SHLIB,
    SHT_DYNSYM,
};

enum reloc_type {
    R_386_NONE,
    R_386_32,
    R_386_PC32,
};

/* dump_symbols()
 *  Print all of the symbols present in the given (valid) ELF executable.
 */
void dump_symbols(struct file *f, struct elf_header *elf);

/* find_symbol()
 *  Return the address of the symbol present in the given (valid) ELF 
 *  executable, or -1 if it was not found/
 */
int find_symbol(struct file *f, struct elf_header *elf, const char *name);

/* load_elf()
 *  Load the ELF executable given by the valid file f into memory and return
 *  its entry point.
 */
struct elf_header *load_elf(struct file *f, page_dir_t *dir, flag_t move);

/* load_kernel_symbols()
 *  Load the kernel ELF executable and load its symbols for tracing.
 */
void load_kernel_symbols(void);

#endif
