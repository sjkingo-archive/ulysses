
#include "../kernel/kernel.h"
#include "cpu.h"

void init_cpu(void)
{
    cpuid_t cpu0 = cpuid(0x00);

    if (cpu0.eax == 0 || cpu0.ebx == 0 || cpu0.ecx == 0 || cpu0.edx == 0)
            panic("cpuid() failed: CPU0 doesn't support it?");

    kprintf("Detected 1 CPU: ebx %d, ecx %d, edx %d\n", cpu0.eax,
            cpu0.ebx, cpu0.ecx, cpu0.edx);
    kprintf("CPU0 stepping %d\n", cpu0.eax);
}

cpuid_t cpuid(unsigned int number) 
{
    cpuid_t result; /* structure to return */

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

