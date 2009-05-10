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
        unsigned int size, unsigned char buffer)
{
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length) {
        return 0;
    }
    
    if (offset + size > header.length) {
        size = header.length - offset;
    }
    
    memcpy(&buffer, (unsigned char *)(header.offset + offset), size);
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

void run_initrd(void)
{
    unsigned int i, loc;

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

    /* Wait for work to come in */
    while (1) kthread_yield();
}
