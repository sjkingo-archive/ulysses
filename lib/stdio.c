/* lib/stdio.c - standard input/output
 * part of Ulysses, a tiny operating system
 *
 * Copyright (C) 2008, 2009 Sam Kingston <sam@sjkwi.com.au>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <sys/types.h>

#if 0
static FILE __stdin = {
    .fd = 0,
    .buffer_pos = 0,
    .buffer_len = 0,
    .buffer = NULL,
};

FILE *stdin = &__stdin;
#endif

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

int feof(FILE *fp)
{
    if (fp->buffer_pos >= fp->buffer_len) return TRUE;
    else return FALSE;
}

char *fgets(char *s, int length, FILE *stream)
{
    int i;

    for (i = 0; i < (length - 1); i++) {
        char c = (char)fgetc(stream);
        if (c == EOF) break;
        s[i] = c;
        if (c == '\n') break;
    }

    if (i == 0) return NULL;

    s[++i] = '\0'; /* don't override newline due to break */
    return s;
}

int fseek(FILE *stream, long offset, int whence)
{
    long pos;
    if (whence == SEEK_SET) pos = offset;
    else if (whence == SEEK_CUR) pos = offset + stream->buffer_pos;
    else if (whence == SEEK_END) pos = offset + stream->buffer_len;
    else return EOF;

    stream->buffer_pos = pos;
    return 0;
}

long ftell(FILE *stream)
{
    return stream->buffer_pos;
}

