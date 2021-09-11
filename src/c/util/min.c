#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    double min = INFINITY, x;
    int i;
    char *endptr;

    if (argc == 1) {
        fputs("At least 1 argument is expected, exiting.\n", stderr);
        return 1;
    }

    for (i = 1; i < argc; i++) {
        errno = 0;
        x = strtod(argv[i], &endptr);

        if (errno) {
            perror("strtod");
            return 2;
        }
        if (argv[i] == endptr) {
            fputs("Invalid number given, exiting.\n", stderr);
            return 2;
        }

        if (x < min)
            min = x;
    }

    printf("%.16g\n", min);

    return 0;
}
