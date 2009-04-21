#ifndef _SHELL_CMDS_H
#define _SHELL_CMDS_H

struct shell_command {
    char *cmd;
    void (*func_noargs)(void);
    void (*func_args)(char **);
};

#endif
