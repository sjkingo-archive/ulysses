
#ifndef _SUICIDE_HALT_H
#define _SUICIDE_HALT_H

#define halt_suicide(); \
        kprintf("\nCPU halt via suicide"); \
        void (*suicide)(void); \
        suicide = (void (*)(void)) - 1; \
        suicide(); \

#endif

