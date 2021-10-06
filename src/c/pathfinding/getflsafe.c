#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "../include/sysutil.h"

#define DIE(ERRMSG) { fputs("getflsafe: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    const char *confdir = "/scripts/pathfinding/files-container/";
    char *prefix, *path, *s, *line;
    size_t size, linesize;
    ssize_t linelen;
    FILE *fl;

    if (argc != 2)
        DIE("invalid argument count");

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir))
            DIE("could not determine config directory");
        prefix = malloc(size = ((strlen(s) + 9) * sizeof(char)));
        snprintf(prefix, size, "%s/.config", s);
    }

    path = malloc(size = (strlen(prefix) + strlen(confdir) + strlen(argv[1]) + 1) * sizeof(char));
    snprintf(path, size, "%s%s%s", prefix, confdir, argv[1]);

    if (!(fl = fopen(path, "rb"))) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    if ((linelen = getdelim(&line, &linesize, '\0', fl)) <= 0)
        DIE("directory database is corrupted, generate a fresh copy");

    rmkfile(line, 0755);

    fputs(line, stdout);

    return 0;
}
