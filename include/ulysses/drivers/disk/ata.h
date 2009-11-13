#ifndef _ULYSSES_ATA_H
#define _ULYSSES_ATA_H

#include <sys/types.h>

struct drive {
    unsigned short num; /* number of drive */
    short cyls; /* number of cylinders */
    short heads; /* number of heads */
    short sectors; /* number of sectors (per track) */
    short bytes_per_sect; /* number of bytes per sector */
    unsigned long size; /* size in bytes */
};

/* Registers */
#define ATA_DATA_REG                0x1F0
#define ATA_SECTOR_COUNT_REG        0x1F2
#define ATA_SECTOR_NUM_REG          0x1F3
#define ATA_CYL_LOW_REG             0x1F4
#define ATA_CYL_HIGH_REG            0x1F5
#define ATA_DRIVE_HEAD_REG          0x1F6
#define ATA_COMMAND_REG             0x1F7
#define ATA_STATUS_REG              0x1F7
#define ATA_DEVICE_CONTROL_REG      0x3F6

/* Drive commands */
#define ATA_COMMAND_READ_SECTORS    0x21
#define ATA_COMMAND_IDENTIFY_DRIVE  0xEC
#define ATA_COMMAND_DIAG            0x90

/* Drives */
#define ATA_DRIVE_0                 0xA0
#define ATA_DRIVE_1                 0xA1

/* Status */
#define ATA_STATUS_DRIVE_ERROR      0x01
#define ATA_STATUS_DRIVE_BUSY       0x80
#define ATA_STATUS_DRIVE_DATA_REQ   0x08

/* Device control register (DCR) */
#define ATA_DCR_NOINT               0x02
#define ATA_DCR_RESET               0x04

/* Drive identification offsets */
#define ATA_ATANT_CYLS              0x01
#define ATA_ATANT_HEADS             0x03
#define ATA_ATANT_SPT               0x06
#define ATA_ATANT_BPS               0x05

/* This loops until the controller declares the drive as "ready" */
#define wait_for_drive() { \
    while ((inb(ATA_STATUS_REG) & ATA_STATUS_DRIVE_BUSY)); \
}

/* Some useful calculations */
#define BYTES_TO_MEGS(b) (b/1000000)
#define BYTES_TO_GIGS(b) (b/1000000000)

void init_ata(void);
void dump_drives(void);
void ata_worker(void);
int read_ata(struct drive *d, int block, char *buffer, size_t size);
struct drive *get_drive(int drive);

#endif
