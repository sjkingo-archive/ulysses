#ifndef _ULYSSES_FS_H
#define _ULYSSES_FS_H

#include <ulysses/fs/ext2.h>
#include <ulysses/fs/fat32.h>

#include <sys/types.h>

flag_t mount_fs(int disk, const char *fs_type);

#endif
