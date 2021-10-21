#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "../include/sysutil.h"

#define DIE(...) { fputs("getpath: ", stderr); \
                   fprintf(stderr, __VA_ARGS__); \
                   exit(EXIT_FAILURE); }

typedef enum {
    SAFETYMODE_UNSAFE = 0,
    SAFETYMODE_NORMAL = 1,
    SAFETYMODE_SAFE = 2,
} SAFETYMODE;

typedef enum {
    FILEMODE_NONE = 0,
    FILEMODE_DIR = 1,
    FILEMODE_FILE = 2,
} FILEMODES;

char *getpath(const char *confdir, const char *keycode)
{
    char *prefix, *path, *s, *line = NULL;
    size_t size, linesize = 0;
    ssize_t linelen;
    FILE *fl;

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir))
            DIE("could not determine config directory\n");
        prefix = malloc(size = ((strlen(s) + 9) * sizeof(char)));
        snprintf(prefix, size, "%s/.config", s);
    }

    path = malloc(size = (strlen(prefix) + strlen(confdir) + strlen(keycode) + 1) * sizeof(char));
    snprintf(path, size, "%s%s%s", prefix, confdir, keycode);

    if (!(fl = fopen(path, "rb"))) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if ((linelen = getdelim(&line, &linesize, '\0', fl)) <= 0)
        DIE("directory database is corrupted, generate a fresh copy\n");

    return line;
}

int main(int argc, char *argv[])
{
    bool b;
    bool optboth = false;
    int i;
    char delim = '\n';
    char *s, *confdir;
    FILEMODES filemodes = FILEMODE_NONE;
    SAFETYMODE safetymode = SAFETYMODE_NORMAL;

    while ((i = getopt(argc, argv, "bdfhsuz0")) != -1) {
        switch (i) {
            case 'b':
                optboth = true;
                break;
            case 'd':
                filemodes |= FILEMODE_DIR;
                break;
            case 'f':
                filemodes |= FILEMODE_FILE;
                break;
            case 'h':
                puts("Usage: getpath -d [OPTION]... [KEYCODE]...\n"
                     "Get paths based on keycodes.\n"
                     "\n"
                     "By default, all parents of the path are created, but the last element is left as-is.\n"
                     "\n"
                     "At least one of -df must be passed.\n"
                     "\n"
                     "  -b        if -d and -f are given, output the results of both\n"
                     "  -d        enable directory mode\n"
                     "  -f        enable file mode\n"
                     "  -h        display this help and exit\n"
                     "  -s        enable safe mode: create all elements of the path\n"
                     "  -u        enable unsafe mode: create none of the elements of the path\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 's':
                safetymode = SAFETYMODE_SAFE;
                break;
            case 'u':
                safetymode = SAFETYMODE_UNSAFE;
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'getpath -d -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (filemodes == FILEMODE_NONE)
        DIE("at least one of -df must be passed");

    argv += optind;
    argc -= optind;

    for (i = 0; i < argc; i++) {
        for (b = false; ; b = !b) {
            errno = 0;

            if (!b) {
                if (!(filemodes & FILEMODE_FILE))
                    continue;
                else
                    confdir = "/scripts/pathfinding/files-container/";
            } else {
                if (!(filemodes & FILEMODE_DIR))
                    break;
                else
                    confdir = "/scripts/pathfinding/directories-container/";
            }

            s = getpath(confdir, argv[i]);

            if (errno) {
                if (s)
                    DIE("%s\n", s);
                return 1;
            }

            switch (safetymode) {
                case SAFETYMODE_UNSAFE:
                    break;
                case SAFETYMODE_NORMAL:
                    rmkparent(s, 0755);
                    break;
                case SAFETYMODE_SAFE:
                    (b ? rmkdir : rmkfile)(s, 0755);
                    break;
            }

            printf("%s%c", s, delim);

            if (b || !optboth)
                break;
        }
    }

    return 0;
}
