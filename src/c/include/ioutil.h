#ifndef HEADER_SCRIPTS_IOUTIL
#define HEADER_SCRIPTS_IOUTIL

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif /* ifndef _POSIX_C_SOURCE */

#include <stdio.h>

char *getstr(int argc, char *argv[], int delim)
{
    static int i = 0;
    static char *line = NULL;
    static size_t size = 0;
    ssize_t len;

    if (argc)
        return i < argc ? argv[i++] : NULL;
    else if ((len = getdelim(&line, &size, delim, stdin)) != -1) {
        if (len && line[len - 1] == delim)
            line[len - 1] = '\0';
        return line;
    }

    return NULL;
}

#endif /* ifndef HEADER_SCRIPTS_IOUTIL */
