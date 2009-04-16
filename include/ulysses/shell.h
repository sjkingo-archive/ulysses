#ifndef _SHELL_H
#define _SHELL_H

#include <sys/types.h>

#define SHELL_BUF_SIZE 1024
#define SHELL_MAX_HISTORY 100

struct _shell {
    int next_pos;
    char data[SHELL_BUF_SIZE];
    char *prompt;
} shell;

/* shell_active
 *  Flag indicating whether shell is currently accepting input to its
 *  data buffer. This is used elseware in the kernel, such as the VT driver.
 */
flag_t shell_active;

/* init_shell()
 *  Initialise the shell data structure and set the prompt as given. This
 *  does not actually run the shell -- call run_shell() for that.
 */
void init_shell(char *prompt);

/* buffer_key()
 *  Buffer a keymap character to the shell's input buffer and write it to
 *  screen.
 */
void buffer_key(const char c);

/* run_shell()
 *  Display the prompt on the current VT and wait for input from the keyboard
 *  driver.
 */
void run_shell(void);

/* run_shell_all()
 *  Display the prompt on all VTs and wait for input from the keyboard
 *  driver.
 */
void run_shell_all(void);

/* shell_history()
 *  Walk through the history and write previous commands to the shell buffer.
 *  If dir is TRUE, walk backwards (from newest to oldest), otherwise walk 
 *  forwards (oldest to newest).
 */
void shell_walk_history(flag_t dir);

#endif
