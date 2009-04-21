
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
    return memcpy(dest, src, (size_t)(strlen(src) + 1));
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

/* Taken straight from Linux's lib/string.c */
char *strpbrk(const char *cs, const char *ct)
{
    const char *sc1, *sc2;

    for (sc1 = cs; *sc1 != '\0'; ++sc1) {
        for (sc2 = ct; *sc2 != '\0'; ++sc2) {
            if (*sc1 == *sc2)
                return (char *)sc1;
        }
    }
    return NULL;
}

/* Taken straight from Linux's lib/string.c */
char *strsep(char **s, const char *ct)
{
    char *sbegin = *s;
    char *end;

    if (sbegin == NULL)
        return NULL;

    end = strpbrk(sbegin, ct);
    if (end)
        *end++ = '\0';
    *s = end;
    return sbegin;
}
