#ifndef _ULYSSES_SHELL_CMDS_H
#define _ULYSSES_SHELL_CMDS_H

/* A shell command */
struct shell_command {
    char *cmd;
    void (*func_noargs)(void);
    void (*func_args)(char **);
    char *desc;
};

#endif
