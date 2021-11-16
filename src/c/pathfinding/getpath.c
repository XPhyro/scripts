#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/param.h>

#include "../include/sysutil.h"

#define DIE(...) { fputs("getpath: ", stderr); \
                   fprintf(stderr, __VA_ARGS__); \
                   if (shellinit) { \
                       switch (argc) { \
                           case 4: \
                               fprintf(stderr, "%s\n", argv[3]); \
                           case 3: \
                               printf("exit %s;\n", argv[2]); \
                               break; \
                           default: \
                               puts("exit 1;\n"); \
                               break; \
                       } \
                   } \
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

int main(int argc, char *argv[])
{
    const char *confdir,
               *const flcont = "/scripts/pathfinding/files-container/",
               *const dircont = "/scripts/pathfinding/directories-container/";
    bool dirturn, optboth = false, optshell = false, shellinit = false;
    int i;
    char delim = '\n', *s, *prefix, *path = NULL, *line = NULL;
    size_t st, size, contmargin, kcmargin, linesize = 0;
    ssize_t linelen;
    FILE *fl;
    FILEMODES filemodes = FILEMODE_NONE;
    SAFETYMODE safetymode = SAFETYMODE_NORMAL;

    while ((i = getopt(argc, argv, "bdefhnsuz0")) != -1) {
        switch (i) {
            case 'b':
                optboth = true;
                break;
            case 'd':
                filemodes |= FILEMODE_DIR;
                break;
            case 'e':
                optshell = true;
                break;
            case 'f':
                filemodes |= FILEMODE_FILE;
                break;
            case 'h':
                puts("Usage: getpath [OPTION]... [KEYCODE]...\n"
                     "  or:  getpath -e [OPTION]... [KEYCODE] [VARNAME] [EXITCODE]? [ERRMSG]?\n"
                     "Get paths based on keycodes.\n"
                     "\n"
                     "EXITCODE and ERRMSG are optional and respectively default to 1 and NULL.\n"
                     "Unless -b is given, the first match of -f and -d in that order is output.\n"
                     "At least one of -df must be given.\n"
                     "Only the last occurence of any of -nsu is considered. By default, -n is selected.\n"
                     "\n"
                     "  -b        if -d and -f are given, output both if matched\n"
                     "  -d        search directory database\n"
                     "  -e        make output `eval`able by a POSIX-compatible shell\n"
                     "  -f        search file database\n"
                     "  -h        display this help and exit\n"
                     "  -n        select normal mode: create parent elements of the path\n"
                     "  -s        select safe   mode: create all elements of the path\n"
                     "  -u        select unsafe mode: create none of the elements of the path\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 'n':
                safetymode = SAFETYMODE_NORMAL;
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
                fputs("Try 'getpath -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if (filemodes == FILEMODE_NONE)
        DIE("at least one of -df must be passed\n");

    argv += optind;
    argc -= optind;

    if (optshell) {
        if (argc < 2 || argc > 4)
            DIE("exactly 2, 3 or 4 positional arguments need to be given when -e is given\n");
        shellinit = true;
    }

    if (!(prefix = getenv("XDG_CONFIG_HOME"))) {
        if (!(s = getenv("HOME")) && !(s = getpwuid(getuid())->pw_dir))
            DIE("could not determine config directory\n");
        prefix = malloc(size = ((strlen(s) + 9) * sizeof(char)));
        snprintf(prefix, size, "%s/.config", s);
    }

    contmargin = strlen(prefix) + MAX(strlen(flcont), strlen(dircont));
    size = 0; /* suppress -Wmaybe-uninitialized */

    if (optshell)
        argc = 1;
    for (i = kcmargin = 0; i < argc; i++) {
        if (!*argv[i])
            DIE("keycode is empty\n");

        if ((st = strlen(argv[i])) > kcmargin)
            path = realloc(path, (size = (contmargin + (kcmargin = st) + 1)) * sizeof(char));

        for (dirturn = false; ; dirturn = !dirturn) {
            if (!dirturn) {
                if (!(filemodes & FILEMODE_FILE))
                    continue;
                confdir = flcont;
            } else {
                if (!(filemodes & FILEMODE_DIR))
                    break;
                confdir = dircont;
            }

            snprintf(path, size, "%s%s%s", prefix, confdir, argv[i]);

            if (!(fl = fopen(path, "rb")))
                DIE("keycode is invalid: %s\n", argv[i]);

            if ((linelen = getdelim(&line, &linesize, '\0', fl)) <= 0)
                DIE("directory database is corrupted, generate a fresh copy\n");

            switch (safetymode) {
                case SAFETYMODE_UNSAFE:
                    break;
                case SAFETYMODE_NORMAL:
                    rmkparent(line, 0755);
                    break;
                case SAFETYMODE_SAFE:
                    (dirturn ? rmkdir : rmkfile)(line, 0755);
                    break;
            }

            if (!optshell) {
                if (argc == 1 && !optboth && !isatty(STDOUT_FILENO))
                    fputs(line, stdout);
                else
                    printf("%s%c", line, delim);
            } else {
                printf("%s='", argv[1]);
                for (s = line; *s; s++) {
                    if (*s != '\'')
                        putchar(*s);
                    else
                        fputs("'\"'\"'", stdout);
                }
                fputs("';\n", stdout);
            }

            if (dirturn || !optboth)
                break;
        }
    }

    return 0;
}
