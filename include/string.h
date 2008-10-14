
#ifndef _STRING_H
#define _STRING_H

#include <sys/types.h>

/* memset()
 *  Fill memory dest with a constant byte c up to len bytes.
 */
void memset(void *dest, int c, size_t len);

/* memcpy()
 *  Copy n bytes from src to dest. The destination must be large enough.
 *  Return a pointer to start of dest.
 */
void *memcpy(void *dest, const void *src, size_t n);

/* strlen()
 *  Calculate length of s, excluding the terminating '\0' char.
 *  Returns the length.
 */
size_t strlen(const char *s);

/* strcpy()
 *  Copy string src into dest. The destination dest must be large enough.
 *  Return a pointer to start of destination.
 */
char *strcpy(char *dest, const char *src);

#endif

