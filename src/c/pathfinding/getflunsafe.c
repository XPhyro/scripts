#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("getflunsafe: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    const char *confdir = "/scripts/pathfinding/files-container/";
    char *prefix, *path, *s;
    unsigned char buf[BUFSIZE];
    size_t size;
    long nread;
    FILE *fl;
    int flno;

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

    errno = 0;
    flno = fileno(fl);
    if (errno || flno == -1) {
        perror("fileno");
        return EXIT_FAILURE;
    }

    while ((nread = read(flno, buf, BUFSIZE)))
        write(STDOUT_FILENO, buf, nread);

    return 0;
}
