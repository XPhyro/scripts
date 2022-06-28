#define _POSIX_C_SOURCE 200809L

#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdutil.h>

#define DIE(...)                       \
    {                                  \
        fputs("expandpath: ", stderr); \
        fprintf(stderr, __VA_ARGS__);  \
        fputc('\n', stderr);           \
        exit(EXIT_FAILURE);            \
    }

char delim = '\n';
char *home;

char *expand(const char *path, char *buf, size_t bufsize)
{
    size_t i, j, len;
    bool dupe = false;
    struct passwd *pw = NULL;

    len = strlen(path);
    buf = areallocfit(buf, bufsize, (len + 1) * sizeof(char));

    for (i = 0, j = 0; i < len; i++) {
        if (path[i] == '/') {
            if (!dupe)
                buf[j++] = '/';
            dupe = true;
        } else {
            buf[j++] = path[i];
            dupe = false;
        }
    }
    buf[len = j] = '\0';

    if (buf[0] != '~') {
        fputs(buf, stdout);
    } else {
        if (len == 1)
            fputs(home, stdout);
        else if (buf[1] == '/') {
            fputs(home, stdout);
            fputs(buf + 1, stdout);
        } else {
            for (i = 0, j = 0; i < len; i++) {
                if (buf[i] == '/') {
                    buf[j = i] = '\0';
                    break;
                }
            }
            if ((pw = getpwnam(buf + 1))) {
                fputs(pw->pw_dir, stdout);
                if (j) {
                    buf[j] = '/';
                    fputs(buf + j, stdout);
                }
            } else {
                if (j)
                    buf[j] = '/';
                fputs(buf, stdout);
            }
        }
    }

    putchar(delim);

    return buf;
}

int main(int argc, char *argv[])
{
    int i;
    char *line = NULL, *buf = NULL;
    size_t size, bufsize = 0;
    ssize_t len;

    while ((i = getopt(argc, argv, "hz0")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage: expandpath [OPTION]... [PATH]...\n"
                     "Like `wordexp`, but only expands tildes.\n"
                     "\n"
                     "With no PATH, read standard input.\n"
                     "\n"
                     "  -h        display this help and exit\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 'z':
            case '0':
                delim = '\0';
                break;
            default:
                fputs("Try 'expandpath -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    argv += optind;
    argc -= optind;

    if (!(home = getenv("HOME")) && !(home = getpwuid(getuid())->pw_dir))
        DIE("could not determine home directory");

    if (!argc) {
        while ((len = getdelim(&line, &size, delim, stdin)) != -1) {
            if (len && line[len - 1] == delim)
                line[len - 1] = '\0';
            buf = expand(line, buf, bufsize);
        }
    } else
        for (i = 0; i < argc; buf = expand(argv[i++], buf, bufsize)) {}

    free(buf);

    return 0;
}
