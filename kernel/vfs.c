
#include "vfs.h"

#include <sys/types.h>

fs_node_t *fs_root = 0;

unsigned int read_fs(fs_node_t *node, unsigned int offset, size_t size, 
        unsigned char *buffer)
{
    if (node->read != 0) {
        return node->read(node, offset, size, buffer);
    } else {
        return 0;
    }
}

unsigned int write_fs(fs_node_t *node, unsigned int offset, size_t size, 
        unsigned char *buffer)
{
    if (node->write != 0) {
        return node->write(node, offset, size, buffer);
    } else {
        return 0;
    }
}

void open_fs(fs_node_t *node, unsigned char read, unsigned char write)
{
    if (node->open != 0) return node->open(node);
}

void close_fs(fs_node_t *node)
{
    if (node->close != 0) return node->close(node);
}

struct dirent *readdir_fs(fs_node_t *node, unsigned int index)
{
    if ((node->flags & 0x7) == FS_DIR && node->readdir != 0) {
        return node->readdir(node, index);
    } else {
        return 0;
    }
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
    if ((node->flags & 0x7) == FS_DIR && node->finddir != 0) {
        return node->finddir(node, name);
    } else {
        return 0;
    }
}

