/* arch/x86/drivers/disk/ext2.c - ext2 file system
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
#include <ulysses/fs/ext2.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/util_x86.h>

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static char *block_cache = NULL;

static inline int offset2block_n(struct ext2_fs *fs, unsigned int offset)
{
    return (offset >> (10 + fs->sb->s_inodes_per_group));
}

static inline int offset2block_n_off(struct ext2_fs *fs, unsigned int offset)
{
    return (offset & (fs->block_size - 1));
}

static inline int inode2group(struct ext2_fs *fs, unsigned int inode)
{
    return ((inode - 1) / fs->sb->s_inodes_per_group);
}

static inline int inode2index(struct ext2_fs *fs, unsigned int inode)
{
    return ((inode - 1) % fs->sb->s_inodes_per_group);
}

static inline int to_offset(struct ext2_fs *fs, int block)
{
    return (block << (10 + fs->sb->s_log_block_size));
}

static struct ext2_inode *ext2_read_inode(struct ext2_fs *fs, unsigned int id)
{
    struct ext2_inode *i = kmalloc(sizeof(struct ext2_inode));
    int group, local_id, block, offset;

    /* Calculate the block offset on disk from this inode # */
    group = inode2group(fs, id);
    local_id = inode2index(fs, id);
    block = (fs->groups[group].bg_inode_table * fs->block_size) + /* absolute */
            (sizeof(struct ext2_inode) * local_id); /* relative */
    offset = to_offset(fs, block);

    /* Read the inode off disk */
    kprintf("reading inode %d (group %d, local id %d) from block %d "
            "(offset %d)\n", id, group, local_id, block, offset);
    if (!read_ata(fs->d, offset, (void *)i, sizeof(struct ext2_inode))) {
        kprintf("Failed to read inode id %d from block %d\n", id, block);
        kfree(i);
        return NULL;
    }

    return i;
}

static void dump_inode(struct ext2_inode *i)
{
    kprintf("inode:");
    if (i->i_mode & EXT2_S_IFDIR) {
        kprintf(" directory");
    } else if (i->i_mode & EXT2_S_IFREG) {
        kprintf(" regular file");
    } else {
        kprintf(" unknown (0x%X)", i->i_mode);
    }

    kprintf(", uid %d, gid %d, ctime %d, size %Ld, links %d, blocks %d, ", 
            i->i_uid, i->i_gid, i->i_ctime, inode_size(i), i->i_links_count,
            i->i_blocks);

    /* permissions */
    char perms_oct[10];
    to_octal(inode_modes(i->i_modes), perms_oct);
    kprintf("perms %s, ", perms_oct);

    if (i->i_flags & EXT2_INDEX_FL) {
        kprintf("hash-index dirent");
    } else {
        kprintf("linked list dirent");
    }
    kprintf("\n");

    /* data blocks */
    int k;
    for (k = 0; k < 15; k++) {
        if (i->i_block[k] == 0) {
            break;
        }
        kprintf("\tb%d: %d\n", k, i->i_block[k]);
    }
}

static flag_t get_file_block(struct ext2_fs *fs, struct ext2_inode *i, 
        unsigned int block)
{
    unsigned int disk_block, indirect_block;

    if (block_cache == NULL) {
        block_cache = kmalloc(fs->block_size);
    }

    if (block < 12) {
        /* direct block */
        disk_block = i->i_block[block];
        if (!read_ata(fs->d, to_offset(fs, disk_block), block_cache,
                    fs->block_size)) {
            return FALSE;
        }

    } else if (block < 12 + (fs->block_size >> 2)) {
        /* single indirection */
        indirect_block = i->i_block[12];
        if (!read_ata(fs->d, to_offset(fs, indirect_block), block_cache,
                    fs->block_size)) {
            return FALSE;
        }
        disk_block = ((unsigned int *)block_cache)[block - 12];
        if (!read_ata(fs->d, to_offset(fs, disk_block), block_cache,
                    fs->block_size)) {
            return FALSE;
        }

    } else {
        kprintf("XXX ext2 double/triple indirection not supported\n");
        return FALSE;
    }

    return TRUE;
}

void ext2_lookup(struct ext2_fs *fs, struct ext2_inode *parent, 
        const char *name)
{
    unsigned int offset;
    int block = 0;

    while (get_file_block(fs, parent, block++)) {
        offset = 0;
        while (offset < fs->block_size) {
            struct ext2_dirent dir;
            memcpy(&dir, block_cache + offset, sizeof(struct ext2_dirent));
            kprintf("dirent: rec_len %d, name_len %d, name '%s'\n", 
                    dir.rec_len, dir.name_len, dir.name);
            offset += dir.rec_len;
        }
    }
}

void ext2_readdir(struct ext2_fs *fs, struct ext2_inode *i, int offset)
{
    int block, block_offset;
    struct ext2_dirent *dir;

    block = offset2block_n(fs, offset);
    block_offset = offset2block_n_off(fs, offset);
    kprintf("offset %d, block %d, block_offset %d\n", offset, block,
            block_offset);

    if (!get_file_block(fs, i, block)) {
        kprintf("Failed to load block\n");
        return;
    }

    dir = kmalloc(sizeof(struct ext2_dirent));
    memcpy(dir, block_cache + block_offset, sizeof(struct ext2_dirent));
    kprintf("dirent: rec_len %d, name_len %d, name '%s'\n", dir->rec_len, 
            dir->name_len, dir->name);

#if 0
    /* retrieve the current directory entry */
    memcpy (&direntry, cache + off_in_blk, __sizeof_direntry);
    /* check that the entry is not corrupted */
    if ((unsigned int)(off_in_blk) + direntry.name_len > __BLK_SIZE) return 0;
    /* copy the entry into the destination direntry  */
    dir->inode = direntry.inode;
    dir->name_len = direntry.name_len;
    memcpy (dir->name, cache + off_in_blk + __sizeof_direntry,
        direntry.name_len);
    dir->name [direntry.name_len] = 0;

    return direntry.rec_len;
#endif
}

#if 0
void read_dir(struct ext2_fs *fs, struct ext2_inode *i, 
        const char *wanted_name)
{
    unsigned int size, offset;
    unsigned long base;
    int block, ent_num;
    char name[EXT2_NAME_LENGTH + 1];
    struct ext2_dirent *dirent = kmalloc(sizeof(struct ext2_dirent));
    
    size = inode_size(i);
    base = i->i_block[0] * fs->block_size;
    block = 0;
    offset = 0;
    ent_num = 0;

    kprintf("size %d, base %ld\n", size, base);
    while (size > 0) {
        if (!read_ata(fs->d, base + offset, dirent, 
                sizeof(struct ext2_dirent))) {
            kprintf("Failed to read dirent for `%s`\n", wanted_name);
            return;
        }
        kprintf("inode %d, rec_len %d, name_len %d\n", dirent->inode,
                dirent->rec_len, dirent->name_len);
    }

#if 0
        if (!read_ata(fs->d, base + offset + sizeof(struct ext2_dirent), 
                name, dirent->name_len)) {
            kprintf("Failed to read dir name from disk\n");
            return;
        }

        if (dirent->name_len != 0) {
            name[dirent->name_len] = '\0';
            kprintf("name `%s`\n", name);
        } else {
            kprintf("nothing\n");
        }

        offset += dirent->rec_len;
        size -= dirent->rec_len;
        ent_num++;
    }
#endif
}
#endif

struct ext2_fs *verify_ext2(struct drive *d)
{
    struct ext2_fs *fs;
    struct ext2_superblock *sb;

    /* Sanity check */
    if (sizeof(struct ext2_superblock) != 1024) {
        kprintf("Someone changed the superblock struct: size %d "
                "(size needs to be 1024)!\n", sizeof(struct ext2_superblock));
        return NULL;
    }

    /* Read the superblock */
    sb = kmalloc(sizeof(struct ext2_superblock));
    if (!read_ata(d, EXT2_SUPERBLOCK_OFFSET/d->bytes_per_sect, (void *)sb, 
            sizeof(struct ext2_superblock))) {
        kprintf("Failed to read superblock off disk\n");
        kfree(sb);
        return NULL;
    }
    if (sb->s_magic != EXT2_MAGIC) {
        kprintf("No ext2 file system here (MAGIC was %X)\n", sb->s_magic);
        kfree(sb);
        return NULL;
    }

    /* Verify we support this type of ext2 file system */
    if (sb->s_rev_level != EXT2_DYNAMIC_REV) {
        kprintf("ext2 file system detected, but it has rev %d "
                "(only rev 1 supported)\n", sb->s_rev_level);
        return NULL;
    }
    
    /* We follow the Linux spec and ignore any other extensions */
    if (sb->s_creator_os != EXT2_OS_LINUX) {
        kprintf("ext2 file system detected, but it was not created with "
                "Linux - cannot continue\n");
        return NULL;
    }

    /* Populate ext2 file system struct */
    fs = kmalloc(sizeof(struct ext2_fs));
    fs->d = d;
    fs->sb = sb;
    fs->group_count = ((sb->s_blocks_count - 1) / sb->s_blocks_per_group) + 1;
    fs->block_size = 1024 << sb->s_log_block_size;

    /* Read groups from disk */
    if (!read_ata(d, to_offset(fs, sb->s_first_data_block + 1), 
            (void *)&fs->groups, sizeof(struct ext2_group) * fs->group_count)) {
        kprintf("Failed to read ext2 groups off disk\n");
        return NULL;
    }

    kprintf("ext2 (rev %d) on disk %d: inodes: %d, first inode: %d, "
            "groups: %d, block size: %d, volume name: '%s'\n", 
            sb->s_rev_level, d->num, sb->s_inodes_count, sb->s_first_ino, 
            fs->group_count, fs->block_size, sb->s_volume_name);

    /* Read the root inode */
    fs->root_inode = ext2_read_inode(fs, EXT2_ROOT_INO);
#if 0
    kprintf("root inode:\n");
    dump_inode(fs->root_inode);
    //read_dir(fs, fs->root_inode, "/");
    //ext2_readdir(fs, fs->root_inode, 524288);
    ext2_lookup(fs, fs->root_inode, "/");
#endif

    return fs;
}
