#ifndef HEADER_SCRIPTS_C_IOUTIL_
#define HEADER_SCRIPTS_C_IOUTIL_

#ifdef _POSIX_C_SOURCE
#define BAK_POSIX_C_SOURCE_ _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE BAK_POSIX_C_SOURCE_
#undef BAK_POSIX_C_SOURCE_
#else /* ifdef _POSIX_C_SOURCE */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#undef _POSIX_C_SOURCE
#endif /* ifndef _POSIX_C_SOURCE */
#include <sys/types.h>

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

#endif /* ifndef HEADER_SCRIPTS_C_IOUTIL_ */
