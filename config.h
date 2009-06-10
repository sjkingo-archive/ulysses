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

/* TIMER_FREQ
 *  Frequency to run the system timer at, in hertz. The higher the frequency,
 *  the more often the timer will tick, and the operating system will be more
 *  responsive. The frequency range of the PIT is fixed at [18..1000] Hz.
 *
 *  1000 Hz = every 1 ms (highest possible)
 *  250 Hz  = every 4 ms
 *  100 Hz  = every 10 ms
 *  50 Hz   = every 50 ms
 *  18 Hz   = every 55 ms (lowest possible)
 */
#define TIMER_FREQ 1000

/* VERSION_NUM
 * VERSION_CN
 *  Version number and codename of the kernel.
*/
#define VERSION_NUM "0.2.0"
#define VERSION_CN "amnesiac"

#endif
