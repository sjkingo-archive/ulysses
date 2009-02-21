
#include "cpu.h"
#include "../../kernel/kernel.h"

static cpuid_t cpuid(unsigned int number) 
{
    cpuid_t result;

    __asm__ volatile(
            "cpuid"
		    : "=a" (result.eax),
		    "=b" (result.ebx),
		    "=c" (result.ecx),
		    "=d" (result.edx)
	        : "0" (number) 
	);

    return result;
}

void init_cpu(void)
{
    cpuid_t cpu0 = cpuid(0x00);
    if (cpu0.eax == 0 || cpu0.ebx == 0 || cpu0.ecx == 0 || cpu0.edx == 0)
            panic("cpuid() failed: CPU0 doesn't support it?");
}

