#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

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

/* path is modified in the process, but restored to original */
void rmkfile(char *path, mode_t mode)
{
    rmkparent(path, mode);
    fopen(path, "a");
}

/* path is modified in the process, but restored to original */
void rmkdir(char *path, mode_t mode)
{
    rmkparent(path, mode);
    mkdir(path, mode);
}
