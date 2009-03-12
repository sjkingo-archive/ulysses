
#include "kernel.h"

#include "shell.h"

#include <string.h>
#include <sys/types.h>

char *last_cmds[SHELL_MAX_HISTORY]; /* array of all commands entered */
int last_cmd_index; /* last index in last_cmds[] array */
int last_up_index; /* last index for walking in history */

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

/* execute_cmd()
 *  Execute the command currently in the buffer. Since strtok() is considered
 *  unsafe for in-kernel use, we assume the entire buffer (up until the NULL
 *  terminator) is a command.
 */
static void execute_cmd(void)
{
    if (strcmp(shell.data, "") == 0) {
        /* no command, ignore */
        last_up_index = last_cmd_index; /* reset */
        return;
    } else if (strcmp(shell.data, "ver") == 0) {
        print_startup();
    } else if (strcmp(shell.data, "uptime") == 0) {
        kprintf("up %d seconds\n", kern.current_time_offset.tv_sec);
    } else if (strcmp(shell.data, "halt") == 0) {
        shutdown();
    } else if (strcmp(shell.data, "check") == 0) {
        sanity_check();
    } else if (strcmp(shell.data, "exit") == 0) {
#if KERN_INTERACTIVE
        startup_kernel();
#else
        shutdown();
#endif
    } else if (strcmp(shell.data, "startup_kernel") == 0) {
        startup_kernel();
    } else if (strcmp(shell.data, "init_proc") == 0) {
        init_proc();
    } else if (strcmp(shell.data, "proc") == 0) {
        print_current_proc();
    } else if (strcmp(shell.data, "cpuinfo") == 0) {
        print_cpuinfo();
    } else if (strcmp(shell.data, "meminfo") == 0) {
        print_meminfo();
    } else if (strcmp(shell.data, "history") == 0) {
        int i;
        for (i = SHELL_MAX_HISTORY; i >= 0; i--) {
            kprintf("%s", last_cmds[i]);
            if (last_up_index == i) kprintf(" <--");
            kprintf("\n");
        }
    } else {
        kprintf("%s: command not found\n", shell.data);
    }

    /* Wrap the history */
    if (last_cmd_index >= SHELL_MAX_HISTORY) {
        last_cmd_index = 0;
    }

    /* Copy the current command into a buffer for reuse */
    last_cmds[last_cmd_index] = (char *)kmalloc(strlen(shell.data));
    strcpy(last_cmds[last_cmd_index], shell.data);
    last_up_index = last_cmd_index++;
}

void init_shell(char *prompt)
{
    shell_active = FALSE;
    shell.next_pos = 0;
    shell.prompt = (char *)kmalloc(strlen(prompt) + 1);
    strcpy(shell.prompt, prompt);

    /* Initialise the history */
    unsigned int i;
    for (i = 0; i < SHELL_MAX_HISTORY; i++) {
        last_cmds[i] = NULL;
    }
    last_cmd_index = 0;
    last_up_index = 0;
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

void shell_walk_history(flag_t dir)
{
    /* Copy the previous history command into the buffer */
    reset_buffer();
    strcpy(shell.data, last_cmds[last_up_index]);
    shell.next_pos = strlen(last_cmds[last_up_index]) + 1;
    
    /* Move the history index if it's not out of bounds */
    if (dir) { /* up, backwards */
        if (last_up_index != 0) last_up_index--;
    } else { /* down, forwards */
        if (last_up_index != last_cmd_index) last_up_index++;
    }

    /* Clear the current line and echo the command */
    clear_last_line();
    run_shell();
    kprintf("%s", shell.data);
}

