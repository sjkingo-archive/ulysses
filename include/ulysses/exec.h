#ifndef _ULYSSES_EXEC
#define _ULYSSES_EXEC

#include <ulysses/task.h>

/* do_execv()
 *  Execute the given file with the given arguments. This replaces the current
 *  process image and will never return -- provided the file could be executed.
 *  The array pointed to by argv *must* have a terminating NULL pointer. If
 *  this ever returns, the file could not be loaded. The return value is
 *  irrelevant (although it will always be -1). errno will be set if an error
 *  occurs.
 */
int do_execv(const char *path, char *const argv[]);

/* create_init()
 *  Create a hand-crafted init process and execute the init file off the
 *  initrd. Returns the process ID of the new task.
 */
pid_t create_init(void);

/* kexec()
 *  This is the kernel version of fork+exec -- the executable is loaded
 *  from initrd and run in the kernel's address space.
 */
pid_t kexec(const char *name);

#endif
