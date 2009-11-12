/* arch/x86/drivers/disk/fat32.c - FAT32 file system
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

#include <ulysses/drivers/disk.h>
#include <ulysses/fs/fat32.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/util_x86.h>

#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FAT32_CLUSTER_SIZE 4096

static inline unsigned int abs_cluster(struct fat32_fs *fs, 
        unsigned int rel_cluster)
{
    return rel_cluster - 2 + fs->first_data_sector;
}

static inline int cluster_to_block(struct fat32_fs *fs, unsigned int cluster)
{
    return ((fs->first_data_sector + (cluster - 2) * 
            fs->bs->sectors_per_cluster) * fs->bs->bytes_per_sector);
}

static unsigned char *read_fat(struct fat32_fs *fs, unsigned int start_sector)
{
    unsigned char *table = kmalloc(FAT32_CLUSTER_SIZE);
    if (!read_ata(fs->d, start_sector, table, FAT32_CLUSTER_SIZE)) {
        kprintf("Couldn't read file allocation table\n");
        return NULL;
    }
    return table;
}

static struct fat_dir_entry *get_dir_entry(struct fat32_fs *fs, 
        unsigned int rel_cluster)
{
    struct fat_dir_entry *dir = kmalloc(sizeof(struct fat_dir_entry));
    if (!read_ata(fs->d, abs_cluster(fs, rel_cluster), dir, 
    //if (!read_ata(fs->d, rel_cluster, dir, 
                sizeof(struct fat_dir_entry))) {
        kfree(dir);
        kprintf("Could not find on disk\n");
        return NULL;
    }

    /* The root entry will have VOLUME_ID and not DIRECTORY set */
    if (dir->attr & ATTR_VOLUME_ID) {
        return dir;
    } else {
        if (dir->dirname[0] == 0x00 || dir->dirname[0] == 0xE5) {
            kprintf("Unused entry\n");
            return NULL;
        } else if (!(dir->attr & ATTR_DIRECTORY)) {
            kprintf("Entry exists, but isn't a dir\n");
            return NULL;
        } else {
            return dir;
        }
    }
}

static int next_cluster(struct fat32_fs *fs, unsigned int current_cluster)
{
    unsigned int offset, sector, ent_offset, n;
    kprintf("finding next for %d\n", current_cluster);

    offset = current_cluster * 4;
    sector = fs->bs->reserved_sector_count + (offset / FAT32_CLUSTER_SIZE);
    ent_offset = offset % FAT32_CLUSTER_SIZE;

    /* Re-read the table */
    fs->table = read_fat(fs, sector);

    /* ignore the high 4 bits */
    n = *(unsigned int *)&fs->table[ent_offset] & 0x0FFFFFFF;
    if (n == 0x00000000) {
        /* unused cluster */
        return 0;
    } else if (n == 0x0FFFFFF7) {
        /* bad cluster */
        return -2;
    } else if (n >= 0x0FFFFFF8) {
        /* end of cluster */
        return -1;
    } else {
        /* pointer to next cluster */
        return n;
    }
}

struct fat32_fs *mount_fat32(struct drive *d)
{
    struct fat_boot_sector *bs = kmalloc(512);
    if (!read_ata(d, 0, bs, 512)) {
        kprintf("Failed to read sector 0\n");
        kfree(bs);
        return NULL;
    }

    /* Treat bootjmp as magic */
    if (bs->bootjmp[0] != 0xEB && bs->bootjmp[0] != 0xE9) {
        kprintf("There is no FAT32 file system here\n");
        kfree(bs);
        return NULL;
    }

    /* null-terminate all of the string fields */
    bs->oem_name[7] = '\0';
    bs->fat32.volume_label[10] = '\0';
    bs->fat32.fat_type_label[5] = '\0';

    /* Verify this is FAT32 */
    if (strcmp((char *)bs->fat32.fat_type_label, "FAT32") != 0) {
        kprintf("There is a FAT file system here, but it's not FAT32: `%s`\n",
                bs->fat32.fat_type_label);
        return NULL;
    }

    /* Populate the file system struct */
    struct fat32_fs *fs = kmalloc(sizeof(struct fat32_fs));
    fs->d = d;
    fs->bs = bs;
    fs->first_data_sector = bs->reserved_sector_count + 
            (bs->table_count * bs->table_size_16);    
    fs->table = read_fat(fs, fs->first_data_sector);

    kprintf("Mounted FAT32 file system with label `%s`\n", 
            bs->fat32.volume_label);
    return fs;
}

static void follow_chain(struct fat32_fs *fs, unsigned int start_cluster)
{
    int next = start_cluster;
    while (next != -1) {
        if (next == 0) {
            kprintf("unused\n");
        } else if (next == -2) {
            kprintf("bad cluster\n");
        } else {
            struct fat_dir_entry *dir = get_dir_entry(fs, next);
            if (dir == NULL) {
                kprintf("not on disk\n");
            } else {
                kprintf("on disk: `%s`\n", dir->dirname);
            }
        }
        next = next_cluster(fs, next);
    }
    kprintf("EOC\n");
}

void list_dirs(struct fat32_fs *fs)
{
    int next, entry = 0;

    /* Grab the root dir first */
    next = fs->bs->fat32.root_cluster;
    struct fat_dir_entry *root = get_dir_entry(fs, next);
    if (root == NULL) {
        kprintf("no root?\n");
        return;
    } else {
        kprintf("found root at rel %d\n", next);
        kprintf("/\n");
    }

    entry = 1;
    while (entry < 256) {
        follow_chain(fs, entry);
        entry++;
    }

#if 0
    next = 0;
    while (next < 256) {
        unsigned int n = *(unsigned int *)&fs->table[next] & 0x0FFFFFFF;
        if (n >= 0x0FFFFFF8) {
            kprintf("next %d: n %X (EOC)\n", next, n);
            next++;
        } else if (n == 0) {
            kprintf("empty\n");
            break;
        } else {
            kprintf("next %d: n %X ", next, n);
            struct fat_dir_entry *dir = get_dir_entry(fs, next);
            if (dir == NULL) {
                kprintf("(not on disk)");
            } else {
                kprintf("(on disk `%s`)", dir->dirname);
            }
            kprintf("\n");
        }
    }

    next = next_cluster(fs, next);
    if (next != -1) {
        kprintf("root isn't EOC?\n");
        return;
    }
    next++;

    /* Iterate over until there are no more to read */
    //while ((next = next_cluster(fs, next)) != -1) {
    while (1) {
        struct fat_dir_entry *dir = get_dir_entry(fs, next);
        if (dir == NULL) {
            kprintf("no entry for rel %d?\n", next);
            next = next_cluster(fs, next);
        } else {
            kprintf("`%s`\n", dir->dirname);
            next = next_cluster(fs, next);
            if (next == -2) {
                /* Bad cluster */
            } else if (next == -1) {
                /* EOC */
                next++;
                continue;
            } else if (next == 0) {
                /* Unused */
            }
        }
    }
#endif
}
