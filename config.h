
#ifndef _CONFIG_H
#define _CONFIG_H

/* Main kernel configuration file. Set any global pre-processor definitions
 * here (all global variables should go in kernel/kernel.h instead).
 */

/* _ARCH_x
 *  The architecture to build the kernel for. x must be a directory in
 *  arch/ and have a corresponding include in kernel/kernel.h for this to
 *  work.
 */
#define _ARCH_x86

/* DEBUG
 *  Whether or not to be verbose in kernel logging.
 */
#define DEBUG 1

#endif

