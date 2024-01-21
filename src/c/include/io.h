#ifndef HEADER_SCRIPTS_C_IO_
#define HEADER_SCRIPTS_C_IO_

#include <stdio.h>
#include <sys/types.h>

#if _POSIX_C_SOURCE >= 200809L
char *fgetstr(FILE *stream, int delim)
{
    static char *line = NULL;
    static size_t size = 0;
    ssize_t len;

    if ((len = getdelim(&line, &size, delim, stream)) != -1) {
        if (len && line[len - 1] == delim)
            line[len - 1] = '\0';
        return line;
    }

    return NULL;
}

char *getstr(int argc, char *argv[], int delim)
{
    static int i = 0;

    if (argc)
        return i < argc ? argv[i++] : NULL;

    return fgetstr(stdin, delim);
}
#endif /* #if _POSIX_C_SOURCE >= 200809L */

#endif /* ifndef HEADER_SCRIPTS_C_IO_ */
