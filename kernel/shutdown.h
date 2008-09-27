
#ifndef _SHUTDOWN_H
#define _SHUTDOWN_H

/* Exit point for the kernel. Here we declare functions for shutting the
 * system down, both cleanly and uncleanly.
 */

/* panic()
 *  Prints the panic message and halts the CPU. Makes no effort to clean up
 *  and shut down processes cleanly.
 */
void panic(const char *msg);

/* shutdown()
 *  Cleanly shuts down KOS.
 */
void shutdown(void);

#endif

