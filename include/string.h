
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

/* strnlen()
 *  Buffer-safe version strlen: return length of s or count, whichever
 *  comes first.
 */
size_t strnlen(const char *s, size_t count);

/* strcpy()
 *  Copy string src into dest. The destination dest must be large enough.
 *  Return a pointer to start of destination.
 */
char *strcpy(char *dest, const char *src);

/* strcmp()
 *   Compares the given strings and returns -1 if s1 is less than s2, 0 if
 *   equal, or 1 if greater than.
 */
int strcmp(const char *s1, const char *s2);

/* strchr()
 *   Returns a pointer to the first instance of the given character in the
 *   string, or NULL if the character could not be found.
 */
char *strchr(const char *s, int c);

/* strpbrk()
 *   Locate the first occurance of any characters in ct in the string cs and
 *   return a pointer to the first location.
 *   Thanks to Linux for this.
 */
char *strpbrk(const char *cs, const char *ct);

/* strsep()
 *   A non-crazy and reentrant version of strtok() taken from Linux.
 */
char *strsep(char **s, const char *ct);

#endif

