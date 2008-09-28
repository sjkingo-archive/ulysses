
#ifndef _COMMON_HALT_H
#define _COMMON_HALT_H

/* Since we don't know what halt instructions exist on the unknown arch,
 * we force the CPU to generate a triple fault. This will halt it, not
 * very nicely mind you...
 */

#include "suicide.h"
#define halt() halt_suicide()

#endif

