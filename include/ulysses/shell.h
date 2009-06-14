#ifndef _ULYSSES_SHELL_H
#define _ULYSSES_SHELL_H

#include <sys/types.h>

#define SHELL_PROMPT "# "
#define SHELL_BUF_SIZE 1024
#define SHELL_MAX_HISTORY 100

struct _shell {
    int next_pos;
    char *data;
} shell;

/* run_shell()
 *  Start the kernel shell.
 */
void run_shell(void);

/* buffer_key()
 *  Buffer a keymap character to the shell's input buffer and write it to
 *  screen.
 */
void buffer_key(const char c);

/* shell_history()
 *  Walk through the history and write previous commands to the shell buffer.
 *  If dir is TRUE, walk backwards (from newest to oldest), otherwise walk 
 *  forwards (oldest to newest).
 */
void shell_walk_history(flag_t dir);

#endif
