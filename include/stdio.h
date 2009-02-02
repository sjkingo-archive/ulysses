
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
 * getc()
 *   Return one character from the given file stream, or EOF if no more 
 *   to be read.
 */
int fgetc(FILE *fp);
#define getc(fp) fgetc(fp)

/* fread()
 *   Copy nmemb items of data of size size from the file stream given
 *   into the array pointed to by ptr. Returns the number of items read.
 */
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

/* feof()
 *   Returns true if the file stream given has reached end of file, else
 *   return false.
 */
int feof(FILE *fp);

/* fgets()
 *  Read a string of characters from the given stream until a newline is
 *  found, or one less than length is reached. Appends a NULL-terminator.
 *  If a newline is found, it is stored in the buffer.
 *  Returns NULL if EOF found with no characters read, else returns s.
 */
char *fgets(char *s, int length, FILE *stream);

#endif

