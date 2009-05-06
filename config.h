
#ifndef _CONFIG_H
#define _CONFIG_H

/* Main kernel configuration file. 
 *
 * This is for global pre-processor definitions only - all global variables 
 * should go in <ulysses/kernel.h> instead.
 */

/* _ARCH_x
 *  The architecture to build the kernel for. x must be a directory in
 *  arch/ with targets set in SConstruct, and have a corresponding include in 
 *  <ulysses/kernel.h> for this to work.
 */
#define _ARCH_x86

/* SCHED_DEBUG
 * TASK_DEBUG
 * INTERRUPT_DEBUG
 *  Whether or not to be verbose in logging.
 */
#define SCHED_DEBUG 0
#define TASK_DEBUG 0
#define INTERRUPT_DEBUG 0

/* KERN_SHELL
 *  The kernel shell is a builtin interpreter running in kernel-space.
 *  If this is set to 1, it will be started once the kernel startup
 *  has been completed.
 */
#define KERN_SHELL 1

/* VERSION_NUM
 * VERSION_CN
 *  Version number and codename of the kernel.
*/
#define VERSION_NUM "0.1.8"
#define VERSION_CN "amnesiac"

#endif

