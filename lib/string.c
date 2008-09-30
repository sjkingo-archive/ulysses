
#include <string.h>
#include <types.h>

void memset(void *dest, int c, size_t len)
{
    char *p = dest;
    while (len--) *p++ = c;
}

