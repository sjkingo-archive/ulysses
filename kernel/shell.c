#include <ulysses/kernel.h>
#include <ulysses/keyboard.h>
#include <ulysses/kheap.h>
#include <ulysses/kthread.h>
#include <ulysses/kprintf.h>
#include <ulysses/screen.h>
#include <ulysses/shell.h>
#include <ulysses/shell_cmds.h>
#include <ulysses/trace.h>
#include <ulysses/vt.h>

#include <string.h>
#include <sys/types.h>

char *last_cmds[SHELL_MAX_HISTORY]; /* array of all commands entered */
int last_cmd_index; /* last index in last_cmds[] array */
int last_up_index; /* last index for walking in history */

extern struct shell_command cmds[];

/* reset_buffer()
 *  Reset the shell data buffer to its start for new data.
 */
static void reset_buffer(void)
{
    TRACE_ONCE;
    
    if (shell.data == NULL) {
        shell.data = kmalloc(SHELL_BUF_SIZE);
    }
    memset(shell.data, 0, SHELL_BUF_SIZE);
    
    shell.next_pos = 0;
}

/* update_history()
 *  Copy whatever is in the shell buffer into the history array.
 */
static void update_history(void)
{
    TRACE_ONCE;
    /* Wrap the history */
    if (last_cmd_index >= SHELL_MAX_HISTORY) {
        last_cmd_index = 0;
    }

    /* Don't add duplicates */
    if (last_cmd_index > 0 && 
            strcmp(shell.data, last_cmds[last_cmd_index - 1]) == 0) {
        return;
    }

    /* Copy the current command into a buffer for reuse */
    last_cmds[last_cmd_index] = (char *)kmalloc(strlen(shell.data));
    strcpy(last_cmds[last_cmd_index], shell.data);
    last_up_index = last_cmd_index++;
}

/* execute_cmd()
 *  Execute the command currently in the buffer. Since strtok() is considered
 *  unsafe for in-kernel use, we assume the entire buffer (up until the NULL
 *  terminator) is a command.
 */
static void execute_cmd(void)
{
    TRACE_ONCE;

    char *cmd, *args;
    unsigned int i;
    
    /* Extract the command */
    cmd = strsep(&shell.data, " ");
    if (cmd == NULL || (strcmp(cmd, "") == 0)) {
        last_up_index = last_cmd_index; /* reset */
        return;
    }
    
    /* Extract the arguments */
    args = strsep(&shell.data, " ");

    /* Find and execute the function */
    i = 0;
    while (cmds[i].cmd != NULL) {
        if (strcmp(cmds[i].cmd, cmd) == 0) {
            if (cmds[i].func_noargs != NULL) {
                cmds[i].func_noargs();
            } else if (cmds[i].func_args != NULL) {
                cmds[i].func_args(&args);
            }
            return;
        }
        i++;
    }

    /* Will only get here if the command couldn't be found */
    kprintf("%s: command not found\n", cmd);
}

static void print_prompt(void)
{
    TRACE_ONCE;
    kprintf("%s", SHELL_PROMPT);
}

void run_shell(void)
{
    TRACE_ONCE;
    shell.next_pos = 0;
    shell.data = NULL;
    reset_buffer();

    /* Initialise the history */
    unsigned int i;
    for (i = 0; i < SHELL_MAX_HISTORY; i++) {
        last_cmds[i] = NULL;
    }
    last_cmd_index = 0;
    last_up_index = 0;

    print_prompt();
    while (1) {
        char c = next_key();
        if (c == 0) {
            kthread_yield();
        } else {
            buffer_key(c);
        }
    }
}

void buffer_key(const char c)
{
    TRACE_ONCE;
    /* Wrap the next_pos if the buffer overflows */
    if ((shell.next_pos + 1) >= SHELL_BUF_SIZE) {
        shell.next_pos = 0;
    }
    
    kprintf("%c", c);
    if (c == '\n') {
        update_history();
        execute_cmd();
        reset_buffer();
        print_prompt();
    } else {
        shell.data[shell.next_pos++] = c;
    }
}

void shell_walk_history(flag_t dir)
{
    TRACE_ONCE;
    /* Copy the previous history command into the buffer */
    reset_buffer();
    strcpy(shell.data, last_cmds[last_up_index]);
    shell.next_pos = strlen(last_cmds[last_up_index]);
    
    /* Move the history index if it's not out of bounds */
    if (dir) { /* up, backwards */
        if (last_up_index != 0) last_up_index--;
    } else { /* down, forwards */
        if (last_up_index != last_cmd_index) last_up_index++;
    }

    /* Clear the current line and echo the command */
    remove_last_line();
    print_prompt();
    kprintf("%s", shell.data);
}
