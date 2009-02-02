
#ifndef _LIMITS_H
#define _LIMITS_H

/* Numerical limits, as specified by SUS 
 * http://www.opengroup.org/onlinepubs/009695399/
 */

/* number of bits in a byte */
#define CHAR_BIT 8

/* signed char */
#define SCHAR_MIN -128
#define SCHAR_MAX 127

/* unsigned char */
#define UCHAR_MAX 255

/* char */
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX

/* multi-byte char */
#define MB_LEN_MAX 4

/* signed short int */
#define SHRT_MIN -32768
#define SHRT_MAX 32767

/* unsigned short int */
#define USHRT_MAX 65535

/* signed int */
#define INT_MIN –2147483648
#define INT_MAX 2147483647

/* unsigned int */
#define UINT_MAX 4294967295

/* signed long int */
#define LONG_MIN -9223372036854775808
#define LONG_MAX 9223372036854775807

/* unsigned long int */
#define ULONG_MAX 18446744073709551615

/* signed long long int */
#define LLONG_MIN -9223372036854775808
#define LLONG_MAX 9223372036854775807

/* unsigned long long int */
#define ULLONG_MAX 18446744073709551615

#endif

