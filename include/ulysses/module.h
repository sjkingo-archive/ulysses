#ifndef _ULYSSES_MODULE
#define _ULYSSES_MODULE

#define MOD_MAX 8
#define MOD_CORE_SYMS_MAX 8

struct module_symbol {
    const char *name;
    int addr;
    int (*func)(void);
};

struct kernel_module {
    const char *name;
    struct module_symbol init;
    struct module_symbol cleanup;
};

/* load_module()
 *  Load a kernel module off the initrd with the given name and execute its
 *  startup handler.
 */
void load_module(const char *name);

#endif
