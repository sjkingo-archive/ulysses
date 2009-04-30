#ifndef _ULYSSES_MULTIBOOT_H
#define _ULYSSES_MULTIBOOT_H

/* This is the Multiboot header file for Intel x86. It declares the structs
 * that a Multiboot loader will store in memory for us. Note that since all
 * the loader will pass us is the physical address to the info struct, the
 * order of each member is important!
 *
 * For anyone wanting to poke this file, make sure you read the spec in
 * *full* first; it is useful!
 *
 * http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */

#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00010003
#define MULTIBOOT_LOADER_MAGIC 0x2BADB002

/* Flag on/off macro */
#define MB_FLAG(flag, bit) ((flag) & (1 << (bit)))

typedef struct multiboot_header {
    unsigned long magic;
    unsigned long flags;
    unsigned long checksum;
    unsigned long header_addr;
    unsigned long load_addr;
    unsigned long load_end_addr;
    unsigned long bss_end_addr;
    unsigned long entry_addr;
} multiboot_header_t;

typedef struct aout_table {
    unsigned long tabsize;
    unsigned long strsize;
    unsigned long addr;
    unsigned long reserved;
} aout_table_t;

typedef struct elf_header {
    unsigned long num;
    unsigned long size;
    unsigned long addr;
    unsigned long shndx;
} elf_header_t;

typedef struct multiboot_info {
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    union {
        aout_table_t aout_sym;
        elf_header_t elf_sec;
    } u;
    unsigned long mmap_length;
    unsigned long mmap_addr;
} multiboot_info_t;

typedef struct memory_map {
    unsigned long size;
    unsigned long base_addr_low;
    unsigned long base_addr_high;
    unsigned long length_low;
    unsigned long length_high;
    unsigned long type;
} memory_map_t;

#endif
