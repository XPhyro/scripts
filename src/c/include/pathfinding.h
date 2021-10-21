#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "sysutil.h"

char *getpath(const char *confdir, const char *keycode)
{
    char *prefix, *path, *s, *line = NULL;
    size_t size, linesize = 0;
    ssize_t linelen;
    FILE *fl;

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir)) {
            errno = 1;
            return "could not determine config directory";
        }
        prefix = malloc(size = ((strlen(s) + 9) * sizeof(char)));
        snprintf(prefix, size, "%s/.config", s);
    }

    path = malloc(size = (strlen(prefix) + strlen(confdir) + strlen(keycode) + 1) * sizeof(char));
    snprintf(path, size, "%s%s%s", prefix, confdir, keycode);

    if (!(fl = fopen(path, "rb"))) {
        perror("fopen");
        return NULL;
    }

    if ((linelen = getdelim(&line, &linesize, '\0', fl)) <= 0) {
        errno = 1;
        return "directory database is corrupted, generate a fresh copy";
    }

    return line;
}

inline char *getdir(const char *keycode)
{
    return getpath("/scripts/pathfinding/directories-container/", keycode);
}

inline char *getfl(const char *keycode)
{
    return getpath("/scripts/pathfinding/files-container/", keycode);
}
