
#include <string.h>
#include <sys/types.h>

void memset(void *dest, int c, size_t len)
{
    char *p = dest;
    while (len--) *p++ = c;
}

size_t strlen(const char *s)
{
    const char *str = s; /* we're butchering the pointer */
    unsigned int count = 0;

    while (*str++) ++count;
    return count;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest; /* to retain start */
    while (*src) *dest++ = *src++;
    return d;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    char *s = (char *)src;
    while (n--) *d++ = *s++;
    return dest;
}

