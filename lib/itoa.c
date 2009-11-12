/* lib/itoa.c - integer to string
 * part of Ulysses, a tiny operating system
 *
 * Shamefully ripped from Minix 3 - thanks AST
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int next;
char qbuf[8];

char *itoa(int n)
{
    register int r, k;
    int flag = 0;
    
    next = 0;
    
    if (n < 0) {
        qbuf[next++] = '-';
        n = -n;
    }
    
    if (n == 0) {
        qbuf[next++] = '0';
    } else {
        k = 10000;
        while (k > 0) {
            r = n / k;
            if (flag || r > 0) {
                qbuf[next++] = '0' + r;
                flag = 1;
            }
            n -= r * k;
            k = k / 10;
        }
    }
    
    qbuf[next] = 0;
    return(qbuf);
}


/* integer (dec, hex) to octal */
inline long to_octal(int n, char *rep)
{
    int r[10], i = 0, j;
    char str[11];
    char *ptr = str;

    while (n > 0) {
        r[i] = n % 8;
        n = n / 8;
        i++;
    }
    i--;

    /* add leading 0 for str - strtol strips it */
    snprintf(ptr, 10 - i, "0");
    ptr++;
    for (j = 0; i >= 0; i--, j++) {
        snprintf(ptr, 10 - i, "%d", r[i]);
        ptr++;
    }

    strcpy(rep, str);
    return strtol(str, NULL, 10);
}
