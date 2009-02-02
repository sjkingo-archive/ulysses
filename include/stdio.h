
#ifndef _STDIO_H
#define _STDIO_H

#include <sys/types.h>

#define EOF -1

typedef struct file_struct {
    int fd;

    unsigned int buffer_pos;
    unsigned int buffer_len;
    char *buffer;
} FILE;

extern FILE *stdin;

/* fgetc()
 *   Return one character from the given file stream, or EOF if no more 
 *   to be read.
 */
int fgetc(FILE *fp);

/* fread()
 *   Copy nmemb items of data of size size from the file stream given
 *   into the array pointed to by ptr. Returns the number of items read.
 */
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif

