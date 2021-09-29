#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#define DIE(ERRMSG) { fputs("min: "ERRMSG"\n", stderr); \
                      exit(EXIT_FAILURE); }

int main(int argc, char *argv[])
{
    double min = INFINITY, x;
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

        if (x < min)
            min = x;
    }

    printf("%.16g\n", min);

    return 0;
}
