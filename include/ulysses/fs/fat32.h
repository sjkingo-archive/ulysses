#ifndef _ULYSSES_FAT32_H
#define _ULYSSES_FAT32_H

#include <ulysses/drivers/disk.h>

/* Attributes */
#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20

/* The FAT32 extensions. Please do not use this structure directly,
 * it is included as the final element (54 bytes) of the fat_boot_sector
 * structure. Access it from there.
 */
struct fat32_boot_sector {
    unsigned int table_size;
    unsigned short extended_flags;
    unsigned short fat_version;
    unsigned int root_cluster;
    unsigned short fat_info;
    unsigned short backup_bs_sector;

    unsigned char reserved_0[12];
    unsigned char drive_number;
    unsigned char reserved_1;

    unsigned char boot_signature;
    unsigned int volume_id;
    unsigned char volume_label[11];
    unsigned char fat_type_label[8]; /* "FAT32  " */
} __attribute__((packed));

struct fat_boot_sector {
    unsigned char bootjmp[3]; /* [0] is either 0xEB or 0xE9 */
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count; /* number fats */
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;
    struct fat32_boot_sector fat32; /* starting at the 36th byte */
} __attribute__((packed));

struct fat_dir_entry {
    unsigned char dirname[11];
    unsigned char attr;
    unsigned char unused_nt; /* only NT uses this */
    unsigned char creation_tenth;
    unsigned short creation_time;
    unsigned short creation_date;
    unsigned short accessed_date;
    unsigned short cluster_high;
    unsigned short modify_date;
    unsigned short modify_time;
    unsigned short cluster_low;
    unsigned int size;
} __attribute__((packed));

struct fat32_fs {
    struct drive *d; /* the drive this fs is on */
    struct fat_boot_sector *bs; /* the boot sector */
    unsigned char *table; /* the file allocation table */
    unsigned int first_data_sector; /* absolute block of first data sector */
};

#endif
