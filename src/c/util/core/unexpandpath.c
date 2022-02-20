#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500

#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/strutil.h"

#define NUSERSINIT 10

typedef struct {
    const char *name;
    const char *home;
    size_t homelen;
} user;

char delim = '\n';
size_t nusers = NUSERSINIT;
user *users;
struct passwd *epasswd = NULL;

void unexpand(const char *path)
{
    static size_t slen = 0;
    static char *s = NULL;
    size_t i, j, len;
    bool dupe = false;

    if (!(len = strlen(path)))
        goto endline;

    if (len > slen)
        s = realloc(s, (len + 1) * sizeof(char));

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
        if (!strneq(users[i].home, s, users[i].homelen))
            continue;
        putchar('~');
        if (!epasswd || !streq(epasswd->pw_name, users[i].name))
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
    int i;
    char *line = NULL, *tmpstr;
    size_t size;
    ssize_t len;
    struct passwd *pw = NULL;
    user u;

    while ((i = getopt(argc, argv, "hsz0")) != -1) {
        switch (i) {
            case 'h':
                puts("Usage: unexpandpath [OPTION]... [PATH]...\n"
                     "Like `wordexp`, but only expands tildes.\n"
                     "\n"
                     "With no PATH, read standard input.\n"
                     "\n"
                     "  -h        display this help and exit\n"
                     "  -s        output '~' instead of '~currentuser'\n"
                     "  -z        line delimiter is NUL, not newline\n"
                     "  -0        line delimiter is NUL, not newline");
                exit(EXIT_SUCCESS);
                break;
            case 's':
                epasswd = getpwuid(getuid());
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

    argv += optind;
    argc -= optind;

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

    if (!argc) {
        while ((len = getdelim(&line, &size, delim, stdin)) != -1) {
            if (len && line[len - 1] == delim)
                line[len - 1] = '\0';
            unexpand(line);
        }
    } else
        for (i = 0; i < argc; i++) {
            unexpand(argv[i]);
        }

    return 0;
}
