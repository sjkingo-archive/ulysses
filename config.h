#ifndef _CONFIG_H
#define _CONFIG_H

/* Main kernel configuration file. 
 *
 * This is for global pre-processor definitions only - all global variables 
 * should go in <ulysses/kernel.h> instead.
 */

/* _ARCH_x
 *  The architecture to build the kernel for. x must be a directory in
 *  arch/ with targets set in SConstruct.
 */
#define _ARCH_x86

/* PREEMPT_KERNEL
 *  If this is set to 1, the scheduler will preempt kernel tasks. If not,
 *  they must yield the CPU before another task switch to occur.
 */
#define PREEMPT_KERNEL 1

/* SCHED_DEBUG
 * TASK_DEBUG
 * INTERRUPT_DEBUG
 *  Whether or not to be verbose in logging.
 */
#define SCHED_DEBUG 0
#define TASK_DEBUG 0
#define INTERRUPT_DEBUG 0

/* LOG_COM1
 *  If this is set to 1, the kernel will duplicate any output to the log VT
 *  to COM1 port for debugging.
 */
#define LOG_COM1 1

/* SHELL_COM2
 *  If this is set to 1, the kernel will attempt to read events from COM2
 *  and interpret them as shell commands.
 */
#define SHELL_COM2 1

/* VERSION_NUM
 * VERSION_CN
 *  Version number and codename of the kernel.
*/
#define VERSION_NUM "0.1.9"
#define VERSION_CN "amnesiac"

#endif
