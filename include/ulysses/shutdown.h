
#ifndef _SHUTDOWN_H
#define _SHUTDOWN_H

/* Exit point for the kernel. Here we declare functions for shutting the
 * system down, both cleanly and uncleanly.
 */

/* panic()
 *  Prints the panic message to all VTs and halts the CPU. 
 *  Makes no effort to clean up and shut down processes cleanly.
 *  The macro exists to insert the file and line number of the callee for
 *  debugging purposes.
 */
#define panic(msg) do_panic(msg, __FILE__, __LINE__)
void do_panic(const char *msg, const char *file, int line);

/* shutdown()
 *  Cleanly shuts down the operating system.
 */
void shutdown(void);

#endif

