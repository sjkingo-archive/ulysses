#ifndef _CTYPE_H
#define _CTYPE_H

#define _U	0x01 /* upper */
#define _L	0x02 /* lower */
#define _D	0x04 /* digit */
#define _C	0x08 /* control char */
#define _P	0x10 /* punctuation */
#define _S	0x20 /* whitespace (space/lf/tab) */
#define _X	0x40 /* hex digit */
#define _SP	0x80 /* hard space (0x20) */

extern unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

unsigned char toupper(unsigned char c);
unsigned char tolower(unsigned char c);

unsigned int numdigits(unsigned int d, unsigned int base);

#endif