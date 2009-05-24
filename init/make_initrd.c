/* make_initrd.c - Create an initrd for the kernel
 * part of Ulysses, a tiny operating system
 *
 * Partially adapted from JamesM's VFS make_initrd.c.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/ulysses/initrd.h"

#define OUTPUT_FILE "./initrd.img"

int main(int argc, char **argv)
{
    unsigned int i, offset, valid_files;
    initrd_file_header_t headers[INITRD_FILENAME_LEN];
    FILE *wstream;

    argc--;
    argv++;

    offset = sizeof(initrd_file_header_t) * INITRD_FILENAME_LEN + sizeof(int);
    printf("header size = %d bytes\n", (int)sizeof(initrd_file_header_t));
    printf("offset = +%d bytes\n", offset);

    /* Add each file to the header */
    valid_files = 0;
    for (i = 0; i < (unsigned int)argc; i++) {
        FILE *stream;
        struct stat s;

        /* Try and open the file */
        if ((stream = fopen(argv[i], "rb")) == NULL) {
            perror("Could not open file");
            fprintf(stderr, "Skipping file %s\n", argv[i]);
            continue;
        }

        stat(argv[i], &s);
        printf("Adding file %s (%d bytes) at 0x%x\n", argv[i], 
                (int)s.st_size, offset);
        valid_files++;
        strcpy(headers[i].name, argv[i]);
        headers[i].offset = offset;
        
        fseek(stream, 0, SEEK_END);
        headers[i].length = ftell(stream);
        offset += headers[i].length;
        headers[i].magic = 0xBF;
        fclose(stream);
    }

    /* Abort now if we didn't find any files to add */
    if (valid_files == 0) {
        fprintf(stderr, "No files could be added, aborting build\n");
        return 1;
    }

    /* Open the output file and write the header to it */
    if ((wstream = fopen(OUTPUT_FILE, "wb")) == NULL) {
        perror("Could not open file");
        return 1;
    }
    fwrite(&argc, sizeof(int), 1, wstream);
    fwrite(headers, sizeof(initrd_file_header_t), INITRD_FILENAME_LEN, 
            wstream);

    /* Write the contents of each file */
    for (i = 0; i < (unsigned int)argc; i++) {
        FILE *stream;
        unsigned char *buf;

        /* If the file doesn't exist now, we just give up since the initrd 
         * will be corrupt already.
         */
        if ((stream = fopen(argv[i], "rb")) == NULL) {
            perror("Could not open file, aborting build");
            return 1;
        }

        /* Read the file and output it to the initrd */
        buf = (unsigned char *)malloc(headers[i].length);
        fread(buf, 1, headers[i].length, stream);
        fwrite(buf, 1, headers[i].length, wstream);

        fclose(stream);
        free(buf);
    }

    fclose(wstream);
    printf("Created %s with %d files\n", OUTPUT_FILE, valid_files);	
    return 0;
}
