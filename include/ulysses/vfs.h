#ifndef _VFS_H
#define _VFS_H

#include <sys/types.h>

#define NR_FILENAME     128

#define FS_FILE         0x01
#define FS_DIR          0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

struct fs_node;

/* Callbacks for read/write etc */
typedef unsigned int (*read_type_t)(struct fs_node *, unsigned int, 
        unsigned int, unsigned char);
typedef unsigned int (*write_type_t)(struct fs_node *, unsigned int, 
        unsigned int, unsigned char);
typedef void (*open_type_t)(struct fs_node *);
typedef void (*close_type_t)(struct fs_node *);
typedef struct dirent * (*readdir_type_t)(struct fs_node *, unsigned int);
typedef struct fs_node * (*finddir_type_t)(struct fs_node *, char *name);

/* A filesystem node */
typedef struct fs_node {
    char name[NR_FILENAME];
    uid_t uid;
    gid_t gid;
    size_t size;
    unsigned int mask; /* permissions mask */
    unsigned int flags;
    unsigned int inode;
    unsigned int impl; /* XXX ?? */

    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;

    struct fs_node *ptr;
} fs_node_t;

struct dirent {
    char name[NR_FILENAME];
    unsigned int ino; /* required by POSIX, XXX find ref */
};

extern fs_node_t *fs_root; /* root of the VFS */

/* These functions deal with file system nodes, so are named differently
 * from their file descriptor counterparts (read/write/close/etc).
 */
unsigned int read_fs(fs_node_t *node, unsigned int offset, size_t size, 
        unsigned char *buffer);
unsigned int write_fs(fs_node_t *node, unsigned int offset, size_t size, 
        unsigned char *buffer);
void open_fs(fs_node_t *node, unsigned char read, unsigned char write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, unsigned int index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif
