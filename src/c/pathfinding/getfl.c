#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../include/pathfinding.h"

#define DIE(...) { fputs("getfl: ", stderr); \
                   fprintf(stderr, __VA_ARGS__); \
                   exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    char *s;

    if (argc != 2)
        DIE("invalid argument count\n");

    errno = 0;
    s = getfl(argv[1]);

    if (errno) {
        if (s)
            DIE("%s\n", s);
        return 1;
    }

    rmkparent(s, 0755);
    fputs(s, stdout);

    return 0;
}
