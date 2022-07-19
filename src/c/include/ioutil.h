#ifndef HEADER_SCRIPTS_IOUTIL
#define HEADER_SCRIPTS_IOUTIL

#ifdef _POSIX_C_SOURCE
#define BAK_POSIX_C_SOURCE _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE BAK_POSIX_C_SOURCE
#undef BAK_POSIX_C_SOURCE
#else /* ifdef _POSIX_C_SOURCE */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#undef _POSIX_C_SOURCE
#endif /* ifndef _POSIX_C_SOURCE */
#include <sys/types.h>

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

#endif /* ifndef HEADER_SCRIPTS_IOUTIL */
