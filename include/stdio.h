
#ifndef _STDIO_H
#define _STDIO_H

#include <sys/types.h>

#define EOF -1

/* As defined by IEEE SUS: http://www.opengroup.org/onlinepubs/009695399/ */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct file_struct {
    int fd;

    unsigned int buffer_pos;
    unsigned int buffer_len;
    char buffer[1024];
} FILE;

extern FILE *stdin;

/* fgetc()
 * getc()
 *   Return one character from the given file stream, or EOF if no more 
 *   to be read.
 */
int fgetc(FILE *fp);
#define getc(fp) fgetc(fp)

/* getchar()
 *   Reads a single character from stdin. See fgetc() for details.
 */
#define getchar() getc(stdin)

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

/* fseek()
 *   Set the file position for the given stream. The new position is
 *   calculated by adding offset bytes to the position given by whence.
 *   This will either be SEEK_SET, SEEK_CUR or SEEK_END, making the offset
 *   relative to start of the file, the current position, or the end of file,
 *   respectively.
 *   Returns 0 for success or -1 if error.
 */
int fseek(FILE *stream, long offset, int whence);

/* rewind()
 *   Sets the file position for the given stream to the start of the
 *   file stream.
 */
#define rewind(stream) fseek(stream, 0, SEEK_SET)

/* ftell()
 *   Returns the current position for the given file stream.
 */
long ftell(FILE *stream);

#endif

