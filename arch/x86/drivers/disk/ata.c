/* arch/x86/drivers/disk/ata.c - ATA driver
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

#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/kprintf.h>
#include <ulysses/shutdown.h>
#include <ulysses/util_x86.h>

#include <ulysses/drivers/disk/ata.h>

#include <sys/types.h>
#include <unistd.h>

#define NUM_DRIVES 2
static struct drive *drives[NUM_DRIVES];

#define READ_INCREM 256
#define IGNORE_OVERFLOW 1

static flag_t ataInit = FALSE;

/* init_drive()
 *  Initialises the given drive and returns a pointer to its struct.
 */
static struct drive *init_drive(int drive)
{
    int status;
    struct drive *d = kmalloc(sizeof(struct drive));

    /* Set the current drive */
    if (drive == 0) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_0);
    } else if (drive == 1) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_1);
    } else {
        return NULL; /* no such controller */
    }

    outb(ATA_COMMAND_REG, ATA_COMMAND_IDENTIFY_DRIVE);
    wait_for_drive();

    status = inb(ATA_STATUS_REG);
    if ((status & ATA_STATUS_DRIVE_DATA_REQ)) {
        /* Drive responded, get its info */
        int i;
        short info[256];

        /* Query the controller for disk information */
        for (i = 0; i < 256; i++) {
            info[i] = inw(ATA_DATA_REG);
        }
        
        /* Populate the struct */
        d->num = drive;
        d->cyls = info[ATA_ATANT_CYLS];
        d->heads = info[ATA_ATANT_HEADS];
        d->sectors = info[ATA_ATANT_SPT];
        d->bytes_per_sect = info[ATA_ATANT_BPS];
        d->size = d->cyls * d->heads * d->sectors * d->bytes_per_sect;
        drives[drive] = d;
        return d;
    } else {
        /* XXX Could be ATAPI */
        kfree(d);
        return NULL;
    }
}

/* num_blocks()
 *  Calculates and returns the number of blocks on the drive.
 */
static inline int num_blocks(struct drive *d)
{
    return d->cyls * d->heads * d->sectors;
}

static flag_t verify_null(const char *buf, size_t len)
{
    unsigned short i;
    for (i = 0; i < len; i++) {
        if (buf[i] != '\0') {
            return FALSE;
        }
    }
    return TRUE;
}

int read_ata(struct drive *d, int block, char *buffer, size_t size)
{
    int sector, cyl, head;
    unsigned short i;
    short *buf = (short *)buffer;

    if (block < 0 || block >= num_blocks(d)) {
        kprintf("Invalid block %d\n", block);
        return FALSE;
    }

    /* Arrrr pirates beware!
     *
     * There is a known buffer overflow here when size < READ_INCREM (which
     * should be 256). This happens because the ATA controller does not support
     * reading less than 256 bytes at a time, and since we can't resize buffer
     * (yet), we must resort to overflowing the end of it. Due to this, you
     * should make sure size is *always* > 256 - otherwise unexpected behaviour
     * may result from calling this function.
     *
     * You have been warrrrrrrned. If you really like disregarding warnings,
     * set IGNORE_OVERFLOW to 1 to disable the following message.
     */
    if (size < READ_INCREM) {
#if !IGNORE_OVERFLOW
        kprintf("read_ata(): warning: possible buffer overflow - requested "
                "size %d\n", size);
#endif
    } else if ((size % READ_INCREM) != 0) {
        kprintf("read_ata(): warning: size (%d) is not a multiple of %d\n", 
                size, READ_INCREM);
    }

    /* Calculate where the block lies on disk */
    sector = block % d->sectors + 1;
    cyl = block / (d->heads * d->sectors);
    head = (block / d->sectors) % d->heads;

    if (sector == 0) {
        kprintf("read_ata(): sector cannot be 0 (block was %d)\n", block);
        return FALSE;
    }

    kdebug("going to read from C%d H%d S%d\n", cyl, head, sector);
    CLI; /* we can't be interrupted while seeking/reading */

    /* Seek the disk */
    outb(ATA_SECTOR_COUNT_REG, 1); /* always 1 sector */
    outb(ATA_SECTOR_NUM_REG, (unsigned char)sector);
    outb(ATA_CYL_LOW_REG, (unsigned char)(LOW_BYTE(cyl)));
    outb(ATA_CYL_HIGH_REG, (unsigned char)(HIGH_BYTE(cyl)));
    if (d->num == 0) {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_0 | head);
    } else {
        outb(ATA_DRIVE_HEAD_REG, ATA_DRIVE_1 | head);
    }
    outb(ATA_COMMAND_REG, ATA_COMMAND_READ_SECTORS);
    wait_for_drive();

    /* Check for error in seeking */
    if (inb(ATA_STATUS_REG) & ATA_STATUS_DRIVE_ERROR) {
        kprintf("read_ata(): error seeking\n");
        STI;
        return FALSE;
    }

    /* Read a full sector */
    for (i = 0; i < READ_INCREM; i++) {
        buf[i] = inw(ATA_DATA_REG);
    }

    STI;
    return TRUE;
}

void init_ata(void)
{
    if (ataInit) panic("init_ata() already called");
    ataInit = TRUE;

    /* Reset the controller */
    outb(ATA_DEVICE_CONTROL_REG, ATA_DCR_NOINT | ATA_DCR_RESET);
    outb(ATA_DEVICE_CONTROL_REG, ATA_DCR_NOINT);
    wait_for_drive();

    unsigned short i;
    for (i = 0; i < NUM_DRIVES; i++) {
        drives[i] = NULL;
    }

    init_drive(0);
}

void dump_drives(void)
{
    unsigned short i;
    for (i = 0; i < NUM_DRIVES; i++) {
        if (drives[i] == NULL) {
            kprintf("ata%d: not connected\n", i);
        } else {
            kprintf("ata%d: %ldM (%d blocks)\n", i, 
                    BYTES_TO_MEGS(drives[i]->size), num_blocks(drives[i]));
        }
    }
}

void ata_worker(void)
{
    if (!ataInit) kthread_exit();
    while (1) {
        kthread_yield();
    }
}

struct drive *get_drive(int drive)
{
    if (drive < 0 || drive >= NUM_DRIVES) {
        return NULL;
    }
    return drives[drive];
}
