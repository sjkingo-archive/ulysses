
#ifndef _CONFIG_H
#define _CONFIG_H

/* Main kernel configuration file. 
 *
 * This is for global pre-processor definitions only - all global variables 
 * should go in kernel/kernel.h instead.
 */

/* _ARCH_x
 *  The architecture to build the kernel for. x must be a directory in
 *  arch/ with targets set in SConstruct, and have a corresponding include in 
 *  kernel/kernel.h for this to work.
 */
#define _ARCH_x86

/* DEBUG
 *  Whether or not to be verbose in kernel logging.
 */
#define DEBUG 1

/* VERSION_NUM
 * VERSION_CN
 *  Version number and codename of the kernel.
*/
#define VERSION_NUM "0.1.5"
#define VERSION_CN "amnesiac"

#endif

