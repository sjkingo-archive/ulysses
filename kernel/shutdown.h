
#ifndef _SHUTDOWN_H
#define _SHUTDOWN_H

/* Functions responsible for shutting down KOS.
 */

void halt(void);
void panic(const char *msg);
void shutdown(void);

#endif

