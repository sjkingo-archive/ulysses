#include <ulysses/cpu.h>
#include <ulysses/kernel.h>
#include <ulysses/kheap.h>
#include <ulysses/shutdown.h>

#include <string.h>

typedef struct cpu_ident cpu_info;
cpu_info model_map[] = {
    { 4, 0, "GenuineIntel", "Intel 486 DX" },
    { 4, 1, "GenuineIntel", "Intel 486 DX" },
    { 4, 2, "GenuineIntel", "Intel 486 SX" },
    { 4, 3, "GenuineIntel", "Intel 487, 486 DX2 or DX2 OverDrive" },
    { 4, 4, "GenuineIntel", "Intel 486 SL" },
    { 4, 5, "GenuineIntel", "Intel 486 SX2" },
    { 4, 7, "GenuineIntel", "Intel 486 DX2 w/ writeback" },
    { 4, 8, "GenuineIntel", "Intel 486 DX4 or DX4 OverDrive" },
    { 5, 1, "GenuineIntel", "Intel Pentium 60/66" },
    { 5, 2, "GenuineIntel", "Intel Pentium 75/90/100/120/133/150/166/200" },
    { 5, 3, "GenuineIntel", "Intel Pentium OverDrive for 486 Systems" },
    { 5, 4, "GenuineIntel", "Intel Pentium w/ MMX 166/200/233" },
    { 6, 1, "GenuineIntel", "Intel Pentium Pro" },
    { 6, 3, "GenuineIntel", "Intel Pentium II" },
    { 6, 5, "GenuineIntel", "Future Intel P6" },
    { 5, 0, "AuthenticAMD", "AMD-K5-PR75/90/100" },
    { 5, 1, "AuthenticAMD", "AMD-K5-PR120/133" },
    { 5, 2, "AuthenticAMD", "AMD-K5-PR150/166" },
    { 5, 3, "AuthenticAMD", "AMD-K5-PR200" },
    { 5, 6, "AuthenticAMD", "AMD-K6 Model 6 (2.9 / 3.2 V Types)" },
    { 5, 7, "AuthenticAMD", "AMD-K6 Model 7 (2.2 V Types)" },
    { 5, 8, "AuthenticAMD", "AMD-K6-2 3D Model 8" },
    { 5, 9, "AuthenticAMD", "AMD-K6-2 3D+ Model 9" },
    { 0, 0, "", ""} /* end marker */
};

static cpuid_t cpuid(unsigned int number) 
{
    cpuid_t result;

    __asm__ __volatile__(
            "cpuid"
		    : "=a" (result.eax),
		    "=b" (result.ebx),
		    "=c" (result.ecx),
		    "=d" (result.edx)
	        : "0" (number) 
	);

    return result;
}

static char *vendor_id(void)
{
    unsigned short i;
    char *str;
    cpuid_t regs;

    str = (char *)kmalloc(12); /* VendorID is fixed to 12 bytes */
    regs = cpuid(0x00);

    for (i = 0; i < 12; i++) {
        if (i < 4) str[i] = ((char *)&regs.ebx)[i];
        else if (i < 8) str[i] = ((char *)&regs.edx)[i-4];
        else if (i < 12) str[i] = ((char *)&regs.ecx)[i-8];
    }
    str[i] = '\0';

    return str;
}

static char *model_id(void)
{
    cpuid_t regs = cpuid(0x01);
    unsigned int family, model, stepping, i;
    char *model_name;

    if (regs.eax < 1) {
        panic("cpuid() failed: CPU doesn't support model info (0x01)");
    }

    family = 0xF & (regs.eax >> 8);
    model = 0xF & (regs.eax >> 4);
    stepping = 0xF & regs.eax;

    /* Loop over the model lookup table until we find a match */
    i = 0;
    while (model_map[i].family != family || model_map[i].model != model) i++;

    /* XXX should really check here to see if there was a match */

    model_name = (char *)kmalloc(64); /* Intel spec says max 64 chars */
    strcpy(model_name, model_map[i].expl);
    
    return model_name;
}

void init_cpu(void)
{
    kern.cpu_vendor = vendor_id();
    kern.cpu_model = model_id();
}
