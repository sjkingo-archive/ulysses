#ifndef _ULYSSES_CPUID_H
#define _ULYSSES_CPUID_H

/* CPU identification.
 * Intel 8086 derivatives provide the CPUID instruction for getting the
 * CPU info.
 */

#define CPUID_VENDOR_LEN 13 /* 12 + \0 */
#define CPUID_VENDOR_AMD "AuthenticAMD"
#define CPUID_VENDOR_INTEL "GenuineIntel"

/* struct to return the results of a CPUID instruction */
typedef struct _cpuid_t {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
} cpuid_t;

/* CPU identification struct. */
struct cpu_ident {
    unsigned int family;
    unsigned int model;
    char vendor[13];
    char expl[64];
};

/* get_cpuid()
 *  Perform a variety of CPUID instructions to get the system's CPU details.
 */
void get_cpuid(void);

#endif
