
#include <stdio.h>
#include <sys/types.h>

static FILE __stdin = {
    .fd = 0,
    .buffer_pos = 0,
    .buffer_len = 0,
    .buffer = NULL,
};

FILE *stdin = &__stdin;

int fgetc(FILE *fp)
{
    if ((fp->buffer_pos + 1) >= fp->buffer_len) return EOF;
    return (int)fp->buffer[fp->buffer_pos++];
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    unsigned long i, els;
    
    if ((els = size*nmemb) <= 0) return 0;

    for (i = 0; i < els; i++) {
        int b = fgetc(stream); /* get the next byte from the stream */

        /* EOF from the stream: add the NULL-terminator and return */
        if (b == EOF) {
            ((char *)ptr)[i] = '\0';
            return i/size; /* partial read */
        }
        
        ((char *)ptr)[i] = (char)b;
    }

    /* Read the maximum given: add the NULL-terminator and return */
    ((char *)ptr)[i] = '\0';
    return nmemb;
}

