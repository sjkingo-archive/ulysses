/* kernel/initrd.c - an initrd for the kernel
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

#include <ulysses/initrd.h>
#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/kprintf.h>
#include <ulysses/kthread.h>
#include <ulysses/vfs.h>

#include <string.h>

initrd_header_t *initrd_header;
initrd_file_header_t *file_headers;
fs_node_t *initrd_root;
fs_node_t *initrd_dev;
fs_node_t *root_nodes;
int nroot_nodes;

struct dirent dirent;

static unsigned int initrd_read(fs_node_t *node, unsigned int offset, 
        unsigned int size, unsigned char *buffer)
{
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length) {
        return 0;
    }
    
    if (offset + size > header.length) {
        size = header.length - offset;
    }
    
    memcpy(buffer, (unsigned char *)(header.offset + offset), size);
    return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, unsigned int index)
{
    if (node == initrd_root && index == 0) {
        strcpy(dirent.name, "dev");
        dirent.name[3] = 0;
        dirent.ino = 0;
        return &dirent;
    }

    if ((int)(index - 1) >= nroot_nodes) {
        return 0;
    }

    strcpy(dirent.name, root_nodes[index - 1].name);
    dirent.name[strlen(root_nodes[index - 1].name)] = 0;
    dirent.ino = root_nodes[index - 1].inode;
    return &dirent;
}

/* initrd_finddir()
 *   Find the given directory in the given file system node and return its
 *   struct. Returns 0 if dir could not be found.
 */
static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
    if (node == initrd_root && !strcmp(name, "dev")) {
        return initrd_dev;
    }

    int i;
    for (i = 0; i < nroot_nodes; i++) {
        if (!strcmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        }
    }

    return 0;
}

struct file *load_file(char *name)
{
    struct file *f;
    fs_node_t *node;
    
    /* Find the node in the fs */
    node = finddir_fs(fs_root, name);
    if (node == NULL) {
        return NULL;
    }

    f = kmalloc(sizeof(struct file));
    f->name = name;

    /* Read the node and check to make sure we read the full file */
    f->data = kmalloc(node->size);
    f->size = read_fs(node, 0, node->size, f->data);
    if (f->size != node->size) {
        kwarn("read %d bytes of %s when node reported length of %d bytes\n", 
                f->size, f->name, node->size);
        return NULL;
    }
    
    return f;
}

void setup_initrd(void)
{
    unsigned int i, loc;

    if (kern.mbi->mods_count == 0) {
        kwarn("boot module not found\n");
        return;
    }

    loc = *(unsigned int *)kern.mbi->mods_addr;
    initrd_header = (initrd_header_t *)loc;
    file_headers = (initrd_file_header_t *) (loc + sizeof(initrd_header_t));

    /* / */
    initrd_root = (fs_node_t *)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = 0;
    initrd_root->uid = 0;
    initrd_root->gid = 0;
    initrd_root->inode = 0;
    initrd_root->size = 0;
    initrd_root->flags = FS_DIR;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;
    fs_root = initrd_root;
    kprintf("initrd: root VFS set up\n");

    /* /dev */
    initrd_dev = (fs_node_t *)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = 0;
    initrd_dev->uid = 0;
    initrd_dev->gid = 0;
    initrd_dev->inode = 0;
    initrd_dev->size = 0;
    initrd_dev->flags = FS_DIR;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = 0;
    initrd_dev->close = 0;
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;
    kprintf("initrd: /dev VFS set up\n");

    root_nodes = (fs_node_t *)kmalloc(sizeof(fs_node_t) * 
            initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;

    for (i = 0; i < initrd_header->nfiles; i++) {
        /* Edit the file's header and alter the relative file offset. */
        file_headers[i].offset += loc;
        strcpy(root_nodes[i].name, file_headers[i].name);
        root_nodes[i].mask = 0;
        root_nodes[i].uid = 0;
        root_nodes[i].gid = 0;
        root_nodes[i].size = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
}
