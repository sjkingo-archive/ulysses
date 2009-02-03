
#ifndef _CPU_H
#define _CPU_H

/* Here is the CPU-specific code for x86.
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

void init_cpu(void);

#endif

