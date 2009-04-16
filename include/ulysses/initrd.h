
#ifndef _INITRD_H
#define _INITRD_H

#include <ulysses/vfs.h>

typedef struct {
    unsigned int nfiles; /* number of files in the ram disk */
} initrd_header_t;

typedef struct {
    unsigned char magic; /* for error checking */
    char name[NR_FILENAME];
    unsigned int offset;
    unsigned int length;
} initrd_file_header_t;

/* init_initrd()
 *   Given the memory address given from multiboot, initialises the ram disk 
 *   and returns a completed file system node.
 */
fs_node_t *init_initrd(unsigned int loc);

#endif

