#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "../include/strutil.h"

#define NUSERSINIT 10

typedef struct {
    const char *name;
    const char *home;
    size_t homelen;
} user;

char delim = '\n';
size_t nusers = NUSERSINIT;
user *users;

void unexpand(const char *path)
{
    char *s;
    size_t i, j, len;
    bool dupe = false;

    len = strlen(path);
    s = malloc((len + 1) * sizeof(char));

    for (i = 0, j = 0; i < len; i++) {
        if (path[i] == '/') {
            if (!dupe)
                s[j++] = '/';
            dupe = true;
        } else {
            s[j++] = path[i];
            dupe = false;
        }
    }
    s[len = j] = '\0';

    for (i = 0; i < nusers; i++) {
        if (!strneq(users[i].home, s, users[i].homelen)) {
            continue;
        }
        putchar('~');
        fputs(users[i].name, stdout);
        if (s[users[i].homelen]) {
            s[users[i].homelen - 1] = '\0';
            fputs(s + users[i].homelen, stdout);
        }
        goto endline;
    }

    fputs(s, stdout);

endline:
    putchar(delim);
}

int main(int argc, char *argv[])
{
    int i, nfiles;
    char *line = NULL, *tmpstr;
    char **files;
    size_t size, len;
    struct passwd *pw = NULL;
    user u;

    while ((i = getopt(argc, argv, "hz0")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage unexpandpath [OPTION]... [PATH]...\n"
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
                fputs("Try 'unexpandpath -h' for more information.\n", stderr);
                exit(EXIT_FAILURE);
                break;
        }
    }

    files  = argv + optind;
    nfiles = argc - optind;

    users = malloc(NUSERSINIT * sizeof(user));

    i = 0;
    while ((pw = getpwent())) {
        if ((len = strlen(pw->pw_dir)) == 1 && *pw->pw_dir == '/')
            continue;

        tmpstr = malloc(len * sizeof(char));
        memcpy(tmpstr, pw->pw_dir, len);
        u.home = tmpstr;
        u.homelen = len;

        tmpstr = malloc((len = strlen(pw->pw_name) + 1) * sizeof(char));
        memcpy(tmpstr, pw->pw_name, len);
        u.name = tmpstr;

        users[i++] = u;

        if (i == nusers)
            users = realloc(users, (nusers *= 2) * sizeof(user));
    }
    users = realloc(users, (nusers = i) * sizeof(user));

    if (!nfiles) {
        while ((len = getdelim(&line, &size, delim, stdin)) != -1) {
            if (len && line[len - 1] == delim)
                line[len - 1] = '\0';
            unexpand(line);
        }
    } else for (i = 0; i < nfiles; i++) {
        unexpand(files[i]);
    }

    return 0;
}
