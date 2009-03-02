
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
    return memcpy(dest, src, strlen(src));
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *d = (char *)dest;
    char *s = (char *)src;
    while (n--) *d++ = *s++;
    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    unsigned char uc1, uc2;

    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }

    uc1 = (*(unsigned char *)s1);
    uc2 = (*(unsigned char *)s2);

    if (uc1 == uc2) {
        return 0;
    } else if (uc1 < uc2) {
        return -1;
    } else {
        return 1;
    }
}

char *strchr(const char *s, int c)
{
    while (*s != '\0') {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }

    return NULL;
}

