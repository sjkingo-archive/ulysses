
#ifndef _STDLIB_H
#define _STDLIB_H

char *itoa(int n);

#define strtol(cp, endp, base) simple_strtol(cp, endp, base)
long simple_strtol(const char *cp, char **endp, unsigned int base);

#endif

