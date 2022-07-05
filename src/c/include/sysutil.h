#ifndef HEADER_SCRIPTS_SYSUTIL
#define HEADER_SCRIPTS_SYSUTIL

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <strutil.h>

/* path is modified in the process, but restored to original */
void rmkparent(char *path, mode_t mode)
{
    size_t i, j, size = strlen(path) + 1;

    for (i = 1, j = 0; i < size; i++) {
        if (path[i] == '/') {
            if (j)
                path[j] = '/';
            path[j = i] = '\0';
            mkdir(path, mode);
        }
    }
    if (j)
        path[j] = '/';
}

void rmkparentconst(const char *path, mode_t mode)
{
    char *pathdup = astrdup(path);
    rmkparent(pathdup, mode);
    free(pathdup);
}

/* path is modified in the process, but restored to original */
void rmkfile(char *path, mode_t mode)
{
    int fd;

    rmkparent(path, mode);
    if ((fd = open(path, O_CREAT, mode)) != -1)
        close(fd);
}

void rmkfileconst(const char *path, mode_t mode)
{
    char *pathdup = astrdup(path);
    rmkfile(pathdup, mode);
    free(pathdup);
}

/* path is modified in the process, but restored to original */
void rmkdir(char *path, mode_t mode)
{
    rmkparent(path, mode);
    mkdir(path, mode);
}

void rmkdirconst(const char *path, mode_t mode)
{
    char *pathdup = astrdup(path);
    rmkdir(pathdup, mode);
    free(pathdup);
}

#endif /* ifndef HEADER_SCRIPTS_SYSUTIL */
