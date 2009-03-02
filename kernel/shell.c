
#include "kernel.h"

#include "shell.h"

#include <string.h>

/* execute_cmd()
 *  Execute the command currently in the buffer. Since strtok() is considered
 *  unsafe for in-kernel use, we assume the entire buffer (up until the NULL
 *  terminator) is a command.
 */
static void execute_cmd(void)
{
    if (strcmp(shell.data, "") == 0) {
        /* no command, ignore */
        return;
    } else if (strcmp(shell.data, "ver") == 0) {
        print_startup();
    } else if (strcmp(shell.data, "uptime") == 0) {
        kprintf("up %d seconds\n", kern.current_time_offset.tv_sec);
    } else {
        kprintf("%s: command not found\n", shell.data);
    }
}

/* reset_buffer()
 *  Reset the shell data buffer to its start for new data.
 */
static void reset_buffer(void)
{
    shell_active = FALSE;
    shell.data[shell.next_pos] = '\0';
    shell.next_pos = 0;
    shell_active = TRUE;
}

void init_shell(char *prompt)
{
    shell_active = FALSE;
    shell.next_pos = 0;
    shell.prompt = (char *)kmalloc(strlen(prompt) + 1);
    strcpy(shell.prompt, prompt);
}

void buffer_key(const char c)
{
    /* Wrap the next_pos if the buffer overflows */
    if ((shell.next_pos + 1) >= SHELL_BUF_SIZE) {
        shell.next_pos = 0;
    }
    
    kprintf("%c", c);
    if (c == '\n') {
        reset_buffer();
        execute_cmd();
        run_shell(); /* give us a new prompt */
    } else {
        shell.data[shell.next_pos++] = c;
    }
}

void run_shell()
{
    kprintf("%s", shell.prompt);
    shell_active = TRUE;
}

