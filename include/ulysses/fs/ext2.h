#ifndef _ULYSSES_FS_EXT2_H
#define _ULYSSES_FS_EXT2_H

#include <sys/types.h>

#include <ulysses/drivers/disk.h>

#define EXT2_NAME_LENGTH 255
#define EXT2_SUPERBLOCK_OFFSET 1024
#define EXT2_MAGIC 0xEF53

#define EXT2_OS_LINUX 0

/* inode modes - format */
#define EXT2_S_IFSOCK   0xC000  /* socket */
#define EXT2_S_IFLNK    0xA000  /* symlink */
#define EXT2_S_IFREG    0x8000  /* regular file */
#define EXT2_S_IFBLK    0x6000  /* block device */
#define EXT2_S_IFDIR    0x4000  /* directory */
#define EXT2_S_IFCHR    0x2000  /* character device */
#define EXT2_S_IFIFO    0x1000  /* fifo */

/* inode modes - execution */
#define EXT2_S_ISUID    0x0800  /* suid */
#define EXT2_S_ISGID    0x0400  /* sgid */
#define EXT2_S_ISVTX    0x0200  /* sticky bit */

/* inode modes - access rights */
#define EXT2_S_IRUSR    0x0100  /* user read */
#define EXT2_S_IWUSR    0x0080  /* user write */
#define EXT2_S_IXUSR    0x0040  /* user execute */
#define EXT2_S_IRGRP    0x0020  /* group read */
#define EXT2_S_IWGRP    0x0010  /* group write */
#define EXT2_S_IXGRP    0x0008  /* group execute */
#define EXT2_S_IROTH    0x0004  /* others read */
#define EXT2_S_IWOTH    0x0002  /* others write */
#define EXT2_S_IXOTH    0x0001  /* others execute */

/* inode flags */
#define EXT2_INDEX_FL   0x00010000 /* hash indexed dirents */

/* reserved inode numbers */
#define EXT2_BAD_INO    1
#define EXT2_ROOT_INO   2

/* ext2 revisions */
#define EXT2_GOOD_OLD_REV 0
#define EXT2_DYNAMIC_REV 1

/* some handy macros */
#define inode_size(i) ((((long long)i->i_dir_acl) << 32) | (i->i_size))
#define inode_modes(m) \
        ((EXT2_S_IRUSR | EXT2_S_IWUSR | EXT2_S_IXUSR) | \
         (EXT2_S_IRGRP | EXT2_S_IWGRP | EXT2_S_IXGRP) | \
         (EXT2_S_IROTH | EXT2_S_IWOTH | EXT2_S_IXOTH))

struct ext2_superblock {
    unsigned int s_inodes_count;
    unsigned int s_blocks_count;
    unsigned int s_r_blocks_count; /* reserved blocks */
    unsigned int s_free_blocks_count;
    unsigned int s_free_inodes_count;
    unsigned int s_first_data_block;
    unsigned int s_log_block_size;
    int s_log_frag_size;
    unsigned int s_blocks_per_group;
    unsigned int s_frags_per_group;
    unsigned int s_inodes_per_group;
    unsigned int s_mtime; /* last? mount time */
    unsigned int s_wtime; /* last? write time */
    unsigned short s_mnt_count; /* times *currently* mounted - update this */
    short s_max_mnt_count; /* max times mounted at once */
    unsigned short s_magic;
    unsigned short s_state;
    unsigned short s_errors;
    unsigned short s_minor_rev_level;
    unsigned int s_lastcheck; /* time of last fs check */
    unsigned int s_checkinterval;
    unsigned int s_creator_os;
    unsigned int s_rev_level;
    unsigned short s_def_resuid; /* default uid for reserved */
    unsigned short s_def_resgid; /* default gid for reserved */

    /* rev 1 stuff */
    unsigned int s_first_ino;
    unsigned short s_inode_size;
    unsigned short s_block_group_nr;
    unsigned int s_feature_compat;
    unsigned int s_feature_incompat;
    unsigned int s_feature_ro_compat;
    unsigned char s_uuid[16];
    unsigned char s_volume_name[16];
    unsigned char s_last_mounted[64];
    unsigned int s_algo_bitmap;

    /* performance stuff */
    unsigned char s_prealloc_blocks;
    unsigned char s_prealloc_dir_blocks;
    unsigned short s_pad1;

    /* journaling for ext3 only */
    unsigned char s_journal_uuid[16];
    unsigned int s_journal_inum;
    unsigned int s_journal_dev;
    unsigned int s_last_orphan;

    /* directory indexing */
    unsigned int s_hash_seed[4];
    unsigned char s_def_hash_version;
    unsigned char s_pad2[3];

    /* other */
    unsigned int s_default_mount_options;
    unsigned int s_first_meta_bg;
    unsigned char s_reserved[760]; /* padding */
} __attribute__((packed));

struct ext2_inode {
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned int i_size;
    unsigned int i_atime;
    unsigned int i_ctime;
    unsigned int i_mtime;
    unsigned int i_dtime;
    unsigned short i_gid;
    unsigned short i_links_count;
    unsigned int i_blocks;
    unsigned int i_flags;
    unsigned int i_osd1;
    unsigned int i_block[15]; /* pointers to blocks */
    unsigned int i_generation;
    unsigned int i_file_acl;
    unsigned int i_dir_acl;
    unsigned int i_faddr;
    unsigned char i_osd2[12];
} __attribute__((packed));

struct ext2_group {
    unsigned int bg_block_bitmap;
    unsigned int bg_inode_bitmap;
    unsigned int bg_inode_table;
    unsigned short bg_free_blocks_count;
    unsigned short bg_free_inodes_count;
    unsigned short bg_used_dirs_count;
    unsigned short bg_pad;
    unsigned int bg_reserved[3];
} __attribute__((packed));

struct ext2_dirent {
    unsigned int inode;
    unsigned short rec_len;
    unsigned char name_len;
    unsigned char file_type;
    unsigned char name[EXT2_NAME_LENGTH];
};

struct ext2_fs {
    struct drive *d;
    struct ext2_superblock *sb;
    struct ext2_group groups[1024];
    unsigned int group_count;
    size_t block_size;

    struct ext2_inode *root_inode;
};

struct ext2_fs *mount_ext2(int drive);
struct ext2_fs *verify_ext2(struct drive *d);

#endif
