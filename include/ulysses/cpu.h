#ifndef _CPU_H
#define _CPU_H

/* Intel 8086 derivatives provide the CPUID instruction for getting the
 * CPU information.
 */

#define CPUID_VENDOR_LEN 13 /* 12 + \0 */
#define CPUID_VENDOR_AMD "AuthenticAMD"
#define CPUID_VENDOR_INTEL "GenuineIntel"

typedef struct _cpuid_t {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
} cpuid_t;

struct cpu_ident {
    unsigned int family;
    unsigned int model;
    char vendor[12];
    char expl[64];
};

void init_cpu(void);

#endif
