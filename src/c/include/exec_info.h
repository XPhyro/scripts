#ifndef HEADER_SCRIPTS_C_EXEC_INFO_
#define HEADER_SCRIPTS_C_EXEC_INFO_

#include <stdio.h>
#include <string.h>

#define DEFINE_EXEC_INFO() \
    const char *execname;  \
    const char *execpath

extern const char *execname;
extern const char *execpath;

void init_exec_info(int argc, char **argv)
{
    char *s;

    if (argc) {
        execpath = argv[0];
        execname = (s = strrchr(execpath, '/')) ? s + 1 : execpath;
    } else {
        execname = "NULL";
        execpath = "NULL";
    }
}

#endif /* ifndef HEADER_SCRIPTS_C_EXEC_INFO_ */
