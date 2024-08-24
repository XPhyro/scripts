#ifndef HEADER_SCRIPTS_C_SYS_
#define HEADER_SCRIPTS_C_SYS_

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <hedley.h>

#include "string.h"

/* path is modified in the process, but restored to original */
void rmkparent(char *path, mode_t mode)
{
    size_t i, len = strlen(path);

    for (i = 1; i < len; i++) {
        if (path[i] == '/' && i != len - 1) {
            path[i] = '\0';
            mkdir(path, mode);
            path[i] = '/';
        }
    }
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

/* no restrict is intentional. buf must be <= path if there is any overlap. */
char *simpslashnbuf(const char *path, char *buf, size_t bufsize)
{
    char c;
    size_t i = 0;
    bool isslash = false;

    while ((c = *path)) {
        if (c == '/') {
            if (isslash) {
                path++;
                continue;
            }
            buf[i] = c;
            isslash = true;
        } else {
            buf[i] = c;
            isslash = false;
        }
        path++;
        i++;
    }
    if (buf[i - 1] == '/')
        buf[i - 1] = '\0';
    else
        buf[i] = '\0';

    return buf;
}

/* no restrict is intentional. buf must be <= path if there is any overlap. */
char *simpslashbuf(const char *path, char *buf)
{
    const char *bufbeg = buf;
    char c;
    bool isslash = false;

    while ((c = *path)) {
        if (c == '/') {
            if (isslash) {
                path++;
                continue;
            }
            *buf = c;
            isslash = true;
        } else {
            *buf = c;
            isslash = false;
        }
        path++;
        buf++;
    }
    if (buf[-1] == '/')
        buf[-1] = '\0';
    else
        *buf = '\0';

    return (char *)bufbeg;
}

char *simpslash(char *path)
{
    simpslashbuf(path, path);
    return path;
}

/* no restrict is intentional. buf must be <= path if there is any overlap. */
void dirslashbuf(char *path, char *buf)
{
    struct stat st;
    size_t size;

    memcpy(buf, path, ((size = strlen(path)) + 1) * sizeof(char));

    if (stat(path, &st) != -1 && S_ISDIR(st.st_mode)) {
        buf[size] = '/';
        buf[size + 1] = '\0';
    }
}

HEDLEY_MALLOC char *dirslash(const char *path)
{
    struct stat st;
    size_t size;
    char *s;

    s = amalloc(((size = strlen(path)) + 2) * sizeof(char));
    memcpy(s, path, size);

    if (stat(path, &st) != -1 && S_ISDIR(st.st_mode)) {
        s[size] = '/';
        s[size + 1] = '\0';
    }

    return s;
}

#endif /* ifndef HEADER_SCRIPTS_C_SYS_ */
