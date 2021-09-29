#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("max: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    double max = -INFINITY, x;
    int i;
    char *endptr;

    if (argc == 1)
        DIE("no argument given");

    for (i = 1; i < argc; i++) {
        errno = 0;
        x = strtod(argv[i], &endptr);

        if (errno) {
            perror("strtod");
            return 2;
        }
        if (argv[i] == endptr)
            DIE("invalid number given");

        if (x > max)
            max = x;
    }

    printf("%.16g\n", max);

    return 0;
}
